#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <time.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "getstream.h"

struct diseqc_cmd {
	struct dvb_diseqc_master_cmd cmd;
	uint32_t wait;
};

struct diseqc_cmd switch_cmds[] = {
	{ { { 0xe0, 0x10, 0x38, 0xf0, 0x00, 0x00 }, 4 }, 0 },
	{ { { 0xe0, 0x10, 0x38, 0xf2, 0x00, 0x00 }, 4 }, 0 },
	{ { { 0xe0, 0x10, 0x38, 0xf1, 0x00, 0x00 }, 4 }, 0 },
	{ { { 0xe0, 0x10, 0x38, 0xf3, 0x00, 0x00 }, 4 }, 0 },
	{ { { 0xe0, 0x10, 0x38, 0xf4, 0x00, 0x00 }, 4 }, 0 },
	{ { { 0xe0, 0x10, 0x38, 0xf6, 0x00, 0x00 }, 4 }, 0 },
	{ { { 0xe0, 0x10, 0x38, 0xf5, 0x00, 0x00 }, 4 }, 0 },
	{ { { 0xe0, 0x10, 0x38, 0xf7, 0x00, 0x00 }, 4 }, 0 },
	{ { { 0xe0, 0x10, 0x38, 0xf8, 0x00, 0x00 }, 4 }, 0 },
	{ { { 0xe0, 0x10, 0x38, 0xfa, 0x00, 0x00 }, 4 }, 0 },
	{ { { 0xe0, 0x10, 0x38, 0xf9, 0x00, 0x00 }, 4 }, 0 },
	{ { { 0xe0, 0x10, 0x38, 0xfb, 0x00, 0x00 }, 4 }, 0 },
	{ { { 0xe0, 0x10, 0x38, 0xfc, 0x00, 0x00 }, 4 }, 0 },
	{ { { 0xe0, 0x10, 0x38, 0xfe, 0x00, 0x00 }, 4 }, 0 },
	{ { { 0xe0, 0x10, 0x38, 0xfd, 0x00, 0x00 }, 4 }, 0 },
	{ { { 0xe0, 0x10, 0x38, 0xff, 0x00, 0x00 }, 4 }, 0 }
};

static inline void msleep(uint32_t msec)
{
	struct timespec req = { msec / 1000, 1000000 * (msec % 1000) };

	while (nanosleep(&req, &req))
		;
}

int diseqc_send_msg (int fd, fe_sec_voltage_t v, struct diseqc_cmd **cmd, fe_sec_tone_mode_t t, fe_sec_mini_cmd_t b) {
	int err;

	if ((err = ioctl(fd, FE_SET_TONE, SEC_TONE_OFF)))
		return err;

	if ((err = ioctl(fd, FE_SET_VOLTAGE, v)))
		return err;

	msleep(15);
	while (*cmd) {
		if ((err = ioctl(fd, FE_DISEQC_SEND_MASTER_CMD, &(*cmd)->cmd)))
			return err;

		msleep((*cmd)->wait);
		cmd++;
	}

	msleep(15);

	if ((err = ioctl(fd, FE_DISEQC_SEND_BURST, b)))
		return err;

	msleep(15);

	return ioctl(fd, FE_SET_TONE, t);
}


int setup_switch (int frontend_fd, int switch_pos, int voltage_18, int hiband) {
	struct diseqc_cmd *cmd[2] = { NULL, NULL };
	int i = 4 * switch_pos + 2 * hiband + (voltage_18 ? 1 : 0);

	if (i < 0 || i >= (int) (sizeof(switch_cmds)/sizeof(struct diseqc_cmd)))
		return -EINVAL;

	cmd[0] = &switch_cmds[i];

	return diseqc_send_msg (frontend_fd,
				i % 2 ? SEC_VOLTAGE_18 : SEC_VOLTAGE_13,
				cmd,
				(i/2) % 2 ? SEC_TONE_ON : SEC_TONE_OFF,
				(i/4) % 2 ? SEC_MINI_B : SEC_MINI_A);
}


/*
 * Dump FrontEnd Status byte as clear text returned
 * by GET_STATUS or GET_EVENT ioctl
 *
 */
char *fe_decode_status(int status) {
	static char	str[256];

	str[0]=0x0;

	if (status & FE_HAS_SIGNAL)
		strcat(str, "HAS_SIGNAL ");
	if (status & FE_HAS_CARRIER)
		strcat(str, "HAS_CARRIER ");
	if (status & FE_HAS_VITERBI)
		strcat(str, "HAS_VITERBI ");
	if (status & FE_HAS_SYNC)
		strcat(str, "HAS_SYNC ");
	if (status & FE_HAS_LOCK)
		strcat(str, "HAS_LOCK ");
	if (status & FE_TIMEDOUT)
		strcat(str, "TIMEDOUT ");
	if (status & FE_REINIT)
		strcat(str, "REINIT ");

	/* Eliminate last space */
	if (strlen(str) > 0)
		str[strlen(str)-1]=0x0;

	return str;
};

static int fe_tune_dvbs(struct adapter_s *adapter) {
	FE_PARAM	feparams;
	int		voltage, tone=SEC_TONE_OFF;

	memset(&feparams, 0, sizeof(FE_PARAM));

	voltage=(adapter->fe.dvbs.t_pol == POL_H) ? SEC_VOLTAGE_18 : SEC_VOLTAGE_13;

	if (adapter->fe.dvbs.t_freq > 2200000) {
		if (adapter->fe.dvbs.lnb_slof) {
			/* Ku Band LNB */
			if (adapter->fe.dvbs.t_freq < adapter->fe.dvbs.lnb_slof) {
				feparams.frequency=(adapter->fe.dvbs.t_freq-
						adapter->fe.dvbs.lnb_lof1);
				tone = SEC_TONE_OFF;
			} else {
				feparams.frequency=(adapter->fe.dvbs.t_freq-
						adapter->fe.dvbs.lnb_lof2);
				tone = SEC_TONE_ON;
			}
		} else {
			/* C Band LNB */
			if (adapter->fe.dvbs.lnb_lof2) {
				if (adapter->fe.dvbs.t_pol == POL_H) {
					feparams.frequency=(adapter->fe.dvbs.lnb_lof2-
						adapter->fe.dvbs.t_freq);
				} else {
					feparams.frequency=(adapter->fe.dvbs.lnb_lof1-
						adapter->fe.dvbs.t_freq);
				}
			} else {
				feparams.frequency=adapter->fe.dvbs.lnb_lof1-adapter->fe.dvbs.t_freq;
			}
		}
	} else {
		feparams.frequency=adapter->fe.dvbs.t_freq;
	}

	feparams.inversion=DVBFE_INVERSION_AUTO;

	if (adapter->type == AT_DVBS2) {
		DVB_SET_DELIVERY(&feparams, DVBFE_DELSYS_DVBS2);
		DVBS2_SET_SYMBOLRATE(&feparams, adapter->fe.dvbs.t_srate);
		DVBS2_SET_FEC(&feparams, DVBFE_FEC_AUTO);
	} else {
		DVB_SET_DELIVERY(&feparams, DVBFE_DELSYS_DVBS);
		DVBS_SET_SYMBOLRATE(&feparams, adapter->fe.dvbs.t_srate);
		DVBS_SET_FEC(&feparams, DVBFE_FEC_AUTO);
	}

	if (adapter->fe.dvbs.lnbsharing) {
		int value=SEC_TONE_OFF;
		logwrite(LOG_DEBUG, "fe: Adapter %d lnb-sharing active - trying to turn off", adapter->no);

		if (ioctl(adapter->fe.fd, FE_SET_TONE, value) < 0) {
			logwrite(LOG_ERROR, "fe: Adapter %d ioctl FE_SET_TONE failed - %s", adapter->no, strerror(errno));
		}

		value=SEC_VOLTAGE_OFF;
		if (ioctl(adapter->fe.fd, FE_SET_VOLTAGE, value) < 0) {
			if (errno == EINVAL) {
				logwrite(LOG_DEBUG, "fe: Adapter %d SEC_VOLTAGE_OFF not possible", adapter->no);
				if (ioctl(adapter->fe.fd, FE_SET_VOLTAGE, voltage) < 0) {
					logwrite(LOG_ERROR, "fe: Adapter %d ioctl FE_SET_VOLTAGE failed - %s", adapter->no, strerror(errno));
				}
			}
		}
	} else if (adapter->fe.dvbs.t_diseqc) {
		if (setup_switch(adapter->fe.fd, adapter->fe.dvbs.t_diseqc-1, voltage, (tone == SEC_TONE_ON))) {
			logwrite(LOG_ERROR, "fe: diseqc failed to send");
#ifdef KLK_SOURCE
                        return -1;                
#else
                        exit(-1);
#endif // KLK_SOURCE

		}
		logwrite(LOG_DEBUG, "fe: diseqc send successful");
		sleep(1);
	} else {
		if (ioctl(adapter->fe.fd, FE_SET_VOLTAGE, voltage) < 0) {
			logwrite(LOG_ERROR, "fe: ioctl FE_SET_VOLTAGE failed - %s", strerror(errno));
		}

		if (ioctl(adapter->fe.fd, FE_SET_TONE, tone) < 0) {
			logwrite(LOG_ERROR, "fe: ioctl FE_SET_TONE failed - %s", strerror(errno));
		}
	}

	logwrite(LOG_INFO, "fe: DVB-S tone = %d", tone);
	logwrite(LOG_INFO, "fe: DVB-S voltage = %d", voltage);
	logwrite(LOG_INFO, "fe: DVB-S diseqc = %d", adapter->fe.dvbs.t_diseqc);
	logwrite(LOG_INFO, "fe: DVB-S freq = %lu", adapter->fe.dvbs.t_freq);
	logwrite(LOG_INFO, "fe: DVB-S lof1 = %lu", adapter->fe.dvbs.lnb_lof1);
	logwrite(LOG_INFO, "fe: DVB-S lof2 = %lu", adapter->fe.dvbs.lnb_lof2);
	logwrite(LOG_INFO, "fe: DVB-S slof = %lu", adapter->fe.dvbs.lnb_slof);
	logwrite(LOG_INFO, "fe: DVB-S feparams.frequency = %d", feparams.frequency);
	logwrite(LOG_INFO, "fe: DVB-S feparams.inversion = %d", feparams.inversion);
	logwrite(LOG_INFO, "fe: DVB-S feparams.u.qpsk.symbol_rate = %d", adapter->fe.dvbs.t_srate);

	if (ioctl(adapter->fe.fd, IOCTL_SET_FE, &feparams) < 0) {
		logwrite(LOG_ERROR, "fe: ioctl FE_SET_FRONTEND failed - %s", strerror(errno));
#ifdef KLK_SOURCE
                return -1;                
#else
                exit(-1);
#endif // KLK_SOURCE
	}

	return 0;
}

int fe_tune_dvbt(struct adapter_s *adapter) {
	FE_PARAM		feparams;

	memset(&feparams, 0, sizeof(FE_PARAM));

	feparams.frequency = adapter->fe.dvbt.freq;
	feparams.inversion = INVERSION_AUTO;

	DVB_SET_DELIVERY(&feparams, DVBFE_DELSYS_DVBT);

	DVBT_SET_CODERATE_HP(&feparams, DVBFE_FEC_AUTO);
	DVBT_SET_CODERATE_LP(&feparams, DVBFE_FEC_AUTO);

	switch(adapter->fe.dvbt.bandwidth) {
#ifdef KLK_SOURCE
		case(3): DVBT_SET_BANDWIDTH(&feparams, DVBFE_BANDWIDTH_AUTO); break;
		case(2): DVBT_SET_BANDWIDTH(&feparams, DVBFE_BANDWIDTH_6_MHZ); break;
		case(1): DVBT_SET_BANDWIDTH(&feparams, DVBFE_BANDWIDTH_7_MHZ); break;
		case(0): DVBT_SET_BANDWIDTH(&feparams, DVBFE_BANDWIDTH_8_MHZ); break;
#else
		case(0): DVBT_SET_BANDWIDTH(&feparams, DVBFE_BANDWIDTH_AUTO); break;
		case(6): DVBT_SET_BANDWIDTH(&feparams, DVBFE_BANDWIDTH_6_MHZ); break;
		case(7): DVBT_SET_BANDWIDTH(&feparams, DVBFE_BANDWIDTH_7_MHZ); break;
		case(8): DVBT_SET_BANDWIDTH(&feparams, DVBFE_BANDWIDTH_8_MHZ); break;
#endif // KLK_SOURCE
		default:
			logwrite(LOG_ERROR, "fe: Unknown DVB-T bandwidth %d", adapter->fe.dvbt.bandwidth);
#ifdef KLK_SOURCE
                        return -1;                
#else
                        exit(-1);
#endif // KLK_SOURCE
	}

	switch(adapter->fe.dvbt.modulation) {
#ifdef KLK_SOURCE
		case(0):DVBT_SET_MODULATION(&feparams, DVBFE_MOD_QPSK); break;
		case(1):DVBT_SET_MODULATION(&feparams, DVBFE_MOD_QAM16); break;
		case(2):DVBT_SET_MODULATION(&feparams, DVBFE_MOD_QAM32); break;
		case(3):DVBT_SET_MODULATION(&feparams, DVBFE_MOD_QAM64); break;
		case(4):DVBT_SET_MODULATION(&feparams, DVBFE_MOD_QAM128); break;
		case(5):DVBT_SET_MODULATION(&feparams, DVBFE_MOD_QAM256); break;
		case(6): DVBT_SET_MODULATION(&feparams, DVBFE_MOD_QAMAUTO); break;
#else
		case(0): DVBT_SET_MODULATION(&feparams, DVBFE_MOD_QAMAUTO); break;
		case(16):DVBT_SET_MODULATION(&feparams, DVBFE_MOD_QAM16); break;
		case(32):DVBT_SET_MODULATION(&feparams, DVBFE_MOD_QAM32); break;
		case(64):DVBT_SET_MODULATION(&feparams, DVBFE_MOD_QAM64); break;
		case(128):DVBT_SET_MODULATION(&feparams, DVBFE_MOD_QAM128); break;
		case(256):DVBT_SET_MODULATION(&feparams, DVBFE_MOD_QAM256); break;
#endif // KLK_SOURCE
		default:
			logwrite(LOG_ERROR, "fe: Unknown DVB-T modulation %d", adapter->fe.dvbt.modulation);
#ifdef KLK_SOURCE
                        return -1;                
#else
                        exit(-1);
#endif // KLK_SOURCE
	}

	switch(adapter->fe.dvbt.tmode) {
#ifdef KLK_SOURCE

		case(2):DVBT_SET_TMODE(&feparams, DVBFE_TRANSMISSION_MODE_AUTO); break;
		case(0):DVBT_SET_TMODE(&feparams, DVBFE_TRANSMISSION_MODE_2K); break;
		case(1):DVBT_SET_TMODE(&feparams, DVBFE_TRANSMISSION_MODE_8K); break;
#else

		case(0):DVBT_SET_TMODE(&feparams, DVBFE_TRANSMISSION_MODE_AUTO); break;
		case(2):DVBT_SET_TMODE(&feparams, DVBFE_TRANSMISSION_MODE_2K); break;
		case(8):DVBT_SET_TMODE(&feparams, DVBFE_TRANSMISSION_MODE_8K); break;
#endif // KLK_SOURCE
		default:
			logwrite(LOG_ERROR, "fe: Unknown DVB-T transmission mode %d", adapter->fe.dvbt.tmode);
#ifdef KLK_SOURCE
                        return -1;                
#else
                        exit(-1);
#endif // KLK_SOURCE
	}

	switch(adapter->fe.dvbt.guard) {
#ifdef KLK_SOURCE
		case(4):DVBT_SET_GUARD(&feparams, DVBFE_GUARD_INTERVAL_AUTO); break;
		case(3):DVBT_SET_GUARD(&feparams, DVBFE_GUARD_INTERVAL_1_4); break;
		case(2):DVBT_SET_GUARD(&feparams, DVBFE_GUARD_INTERVAL_1_8); break;
		case(1):DVBT_SET_GUARD(&feparams, DVBFE_GUARD_INTERVAL_1_16); break;
		case(0):DVBT_SET_GUARD(&feparams, DVBFE_GUARD_INTERVAL_1_32); break;
#else
		case(0):DVBT_SET_GUARD(&feparams, DVBFE_GUARD_INTERVAL_AUTO); break;
		case(4):DVBT_SET_GUARD(&feparams, DVBFE_GUARD_INTERVAL_1_4); break;
		case(8):DVBT_SET_GUARD(&feparams, DVBFE_GUARD_INTERVAL_1_8); break;
		case(16):DVBT_SET_GUARD(&feparams, DVBFE_GUARD_INTERVAL_1_16); break;
		case(32):DVBT_SET_GUARD(&feparams, DVBFE_GUARD_INTERVAL_1_32); break;
#endif // KLK_SOURCE

		default:
			logwrite(LOG_ERROR, "fe: Unknown DVB-T guard interval %d", adapter->fe.dvbt.guard);
#ifdef KLK_SOURCE
                        return -1;                
#else
                        exit(-1);
#endif // KLK_SOURCE
	}

	switch(adapter->fe.dvbt.hierarchy) {
#ifdef KLK_SOURCE
		case(0):DVBT_SET_HIERARCHY(&feparams, DVBFE_HIERARCHY_OFF); break;
		case(4):DVBT_SET_HIERARCHY(&feparams, DVBFE_HIERARCHY_AUTO); break;
		case(1):DVBT_SET_HIERARCHY(&feparams, DVBFE_HIERARCHY_1); break;
		case(2):DVBT_SET_HIERARCHY(&feparams, DVBFE_HIERARCHY_2); break;
		case(3):DVBT_SET_HIERARCHY(&feparams, DVBFE_HIERARCHY_4); break;
#else
		case(-1):DVBT_SET_HIERARCHY(&feparams, DVBFE_HIERARCHY_OFF); break;
		case(0):DVBT_SET_HIERARCHY(&feparams, DVBFE_HIERARCHY_AUTO); break;
		case(1):DVBT_SET_HIERARCHY(&feparams, DVBFE_HIERARCHY_1); break;
		case(2):DVBT_SET_HIERARCHY(&feparams, DVBFE_HIERARCHY_2); break;
		case(4):DVBT_SET_HIERARCHY(&feparams, DVBFE_HIERARCHY_4); break;

#endif // KLK_SOURCE

		default:
			logwrite(LOG_ERROR, "fe: Unknown DVB-T hierarchy %d", adapter->fe.dvbt.hierarchy);
#ifdef KLK_SOURCE
                        return -1;                
#else
                        exit(-1);
#endif // KLK_SOURCE
	}

	if (ioctl(adapter->fe.fd, IOCTL_SET_FE, &feparams) < 0) {
		logwrite(LOG_ERROR, "ioctl FE_SET_FRONTEND failed");
#ifdef KLK_SOURCE
                return -1;                
#else
                exit(-1);
#endif // KLK_SOURCE
	}

	return 0;
}

static int fe_tune_dvbc(struct adapter_s *adapter) {
	FE_PARAM	feparams;

	memset(&feparams, 0, sizeof(FE_PARAM));

	feparams.frequency = adapter->fe.dvbc.freq;

	DVB_SET_DELIVERY(&feparams, DVBFE_DELSYS_DVBC);
	DVBC_SET_SYMBOLRATE(&feparams, adapter->fe.dvbc.srate);

	feparams.inversion=INVERSION_AUTO;

	switch(adapter->fe.dvbc.modulation) {
		case -1: DVBC_SET_MODULATION(&feparams, DVBFE_MOD_QPSK); break;
		case 0: DVBC_SET_MODULATION(&feparams, DVBFE_MOD_QAMAUTO); break;
		case 16: DVBC_SET_MODULATION(&feparams, DVBFE_MOD_QAM16); break;
		case 32: DVBC_SET_MODULATION(&feparams, DVBFE_MOD_QAM32); break;
		case 64: DVBC_SET_MODULATION(&feparams, DVBFE_MOD_QAM64); break;
		case 128: DVBC_SET_MODULATION(&feparams, DVBFE_MOD_QAM128); break;
		case 256: DVBC_SET_MODULATION(&feparams, DVBFE_MOD_QAM256); break;
		default:
			logwrite(LOG_ERROR, "Unknown modulation %d", adapter->fe.dvbc.modulation);
#ifdef KLK_SOURCE
                        return -1;                
#else
                        exit(-1);
#endif // KLK_SOURCE
	}
	switch(adapter->fe.dvbc.fec) {
		case 0: DVBC_SET_FEC(&feparams, DVBFE_FEC_NONE); break;
		case 1: DVBC_SET_FEC(&feparams, DVBFE_FEC_1_2); break;
		case 2: DVBC_SET_FEC(&feparams, DVBFE_FEC_2_3); break;
		case 3: DVBC_SET_FEC(&feparams, DVBFE_FEC_3_4); break;
		case 4: DVBC_SET_FEC(&feparams, DVBFE_FEC_4_5); break;
		case 5: DVBC_SET_FEC(&feparams, DVBFE_FEC_5_6); break;
		case 6: DVBC_SET_FEC(&feparams, DVBFE_FEC_6_7); break;
		case 7: DVBC_SET_FEC(&feparams, DVBFE_FEC_7_8); break;
		case 8: DVBC_SET_FEC(&feparams, DVBFE_FEC_8_9); break;
		case 9: DVBC_SET_FEC(&feparams, DVBFE_FEC_AUTO); break;
		default:
			logwrite(LOG_ERROR, "Unknown fec %d", adapter->fe.dvbc.fec);
#ifdef KLK_SOURCE
                        return -1;                
#else
                        exit(-1);
#endif // KLK_SOURCE
	}

	if (ioctl(adapter->fe.fd, IOCTL_SET_FE, &feparams) < 0) {
		logwrite(LOG_ERROR, "ioctl FE_SET_FRONTEND failed");
#ifdef KLK_SOURCE
                return -1;                
#else
                exit(-1);
#endif // KLK_SOURCE
	}

	return 0;
}

#ifdef KLK_SOURCE
static int fe_tune(struct adapter_s *adapter) {
        int rc = 0;    
	switch(adapter->type) {
		case(AT_DVBS2):
		case(AT_DVBS):
			rc = fe_tune_dvbs(adapter);
			break;
		case(AT_DVBT):
			rc = fe_tune_dvbt(adapter);
			break;
		case(AT_DVBC):
			rc = fe_tune_dvbc(adapter);
			break;
	}

	adapter->fe.tunelast=time(NULL);

	return rc;
}
#else
static int fe_tune(struct adapter_s *adapter) {
	switch(adapter->type) {
		case(AT_DVBS2):
		case(AT_DVBS):
			fe_tune_dvbs(adapter);
			break;
		case(AT_DVBT):
			fe_tune_dvbt(adapter);
			break;
		case(AT_DVBC):
			fe_tune_dvbc(adapter);
			break;
	}

	adapter->fe.tunelast=time(NULL);

	return 0;
}
#endif

#define FE_TUNE_MINDELAY	5

void fe_retune(struct adapter_s *adapter) {
	time_t		now;

	now=time(NULL);

	/* Debounce the retuning */
	if (adapter->fe.tunelast + FE_TUNE_MINDELAY > now)
		return;

	fe_tune(adapter);
}

static void fe_timer_init(struct adapter_s *adapter);

static void fe_check_status(int fd, short event, void *arg) {
	struct adapter_s	*adapter=arg;
	fe_status_t		status;
	int			res;

	res=ioctl(adapter->fe.fd, FE_READ_STATUS, &status);

	if (res == 0) {
		if (!(status & FE_HAS_LOCK)) {
			logwrite(LOG_INFO, "fe: Adapter %d Status: 0x%02x (%s)",
					adapter->no, status, fe_decode_status(status));
			fe_retune(adapter);
		}
	}

	fe_timer_init(adapter);
}

#define FE_CHECKSTATUS_INTERVAL	5

static void fe_timer_init(struct adapter_s *adapter) {
	struct timeval	tv;

	tv.tv_sec=FE_CHECKSTATUS_INTERVAL;
	tv.tv_usec=0;

	evtimer_set(&adapter->fe.timer, fe_check_status, adapter);
#ifdef KLK_SOURCE
        event_base_set(adapter->klkbase, &adapter->fe.timer);        
#endif // KLK_SOURCE
	evtimer_add(&adapter->fe.timer, &tv);
}

/*
 * We had an event on the frontend filedescriptor - poll the event
 * and dump the status
 *
 */
static void fe_event(int fd, short ev, void *arg) {
	struct adapter_s		*adapter=arg;
	int				res, status;
	FE_EVENT			event;

	res=ioctl(adapter->fe.fd, IOCTL_GET_EVENT, &event);

	if (res < 0 && errno != EOVERFLOW) {
		logwrite(LOG_ERROR, "fe: Adapter %d Status event overflow %d",
				adapter->no, errno);
		return;
	}

	status=FE_GET_STATUS(event);

	if (res >= 0 && status) {
		if (!(status & FE_TIMEDOUT)) {

			logwrite(LOG_INFO, "fe: Adapter %d Status: 0x%02x (%s)",
				adapter->no, status, fe_decode_status(status));

			if (!(status & FE_HAS_LOCK)) {
				fe_retune(adapter);
			}
		}
	}
}

#ifdef MULTIPROTO

#ifdef KLK_SOURCE
static int fe_checkcap(struct adapter_s *adapter) {
#else
static void fe_checkcap(struct adapter_s *adapter) {
#endif // KLK_SOURCE

	/* With multiproto API changes from 2008-03-09 one has to set the
	 * delsys before anything else
	 */
	if (ioctl(adapter->fe.fd, IOCTL_SET_DELSYS, &adapter->type)) {
		logwrite(LOG_ERROR, "fe: failed to set delivery system with IOCTL_SET_DELSYS with %s", strerror(errno));
#ifdef KLK_SOURCE
                return -1;                
#else
                exit(-1);
#endif // KLK_SOURCE
	}

	if (ioctl(adapter->fe.fd, IOCTL_GET_INFO, &adapter->fe.feinfo)) {
		logwrite(LOG_ERROR, "fe: ioctl(DVBFE_GET_INFO...) failed");
#ifdef KLK_SOURCE
                return -1;                
#else
                exit(-1);
#endif // KLK_SOURCE
	}

	logwrite(LOG_DEBUG, "fe: adapter %d delivery type %d name \"%s\"",
				adapter->no,
				adapter->type,
				adapter->fe.feinfo.name);

	logwrite(LOG_DEBUG, "fe: adapter %d frequency min %d max %d step %d tolerance %d",
				adapter->no,
				adapter->fe.feinfo.frequency_min,
				adapter->fe.feinfo.frequency_max,
				adapter->fe.feinfo.frequency_step,
				adapter->fe.feinfo.frequency_tolerance);

	logwrite(LOG_DEBUG, "fe: adapter %d symbol rate min %d max %d tolerance %d",
				adapter->no,
				adapter->fe.feinfo.symbol_rate_min,
				adapter->fe.feinfo.symbol_rate_max,
				adapter->fe.feinfo.symbol_rate_tolerance);

#ifdef KLK_SOURCE
        return 0;        
#endif // KLK_SOURCE
}

#else

#ifdef KLK_SOURCE
static int fe_checkcap(struct adapter_s *adapter) {
#else
static void fe_checkcap(struct adapter_s *adapter) {
#endif // KLK_SOURCE
	char		*type="unknown";

	if (ioctl(adapter->fe.fd, IOCTL_GET_INFO, &adapter->fe.feinfo)) {
		logwrite(LOG_ERROR, "fe: ioctl(FE_GET_INFO...) failed");
#ifdef KLK_SOURCE
                return -1;                
#else
                exit(-1);
#endif // KLK_SOURCE
	}

	switch(adapter->fe.feinfo.type) {
		case(FE_QPSK):
			type="QPSK";
			if (adapter->type == AT_DVBS)
				break;
			logwrite(LOG_ERROR, "fe: Adapter %d is an DVB-S card - config is not for DVB-S", adapter->no);
#ifdef KLK_SOURCE
                        return -1;                
#else
                        exit(-1);
#endif // KLK_SOURCE
		case(FE_OFDM):
			type="OFDM";
			if (adapter->type == AT_DVBT)
				break;
			logwrite(LOG_ERROR, "fe: Adapter %d is an DVB-T card - config is not for DVB-T", adapter->no);
#ifdef KLK_SOURCE
                        return -1;                
#else
                        exit(-1);
#endif // KLK_SOURCE
		case(FE_QAM):
			type="QAM";
			if (adapter->type == AT_DVBC)
				break;
			logwrite(LOG_ERROR, "fe: Adapter %d is an DVB-C card - config is not for DVB-C", adapter->no);
#ifdef KLK_SOURCE
                        return -1;                
#else
                        exit(-1);
#endif // KLK_SOURCE
		default:
			logwrite(LOG_ERROR, "fe: Adapter %d is an unknown card type %d", adapter->no, adapter->fe.feinfo.type);
			break;
	}

	if (adapter->fe.feinfo.type == FE_QPSK && !(adapter->fe.feinfo.caps & FE_CAN_FEC_AUTO)) {
		logwrite(LOG_ERROR, "fe: adapter %d is incapable of handling FEC_AUTO - please report to flo@rfc822.org", adapter->no);
	}

	logwrite(LOG_DEBUG, "fe: adapter %d type %s name \"%s\"",
				adapter->no,
				type,
				adapter->fe.feinfo.name);

	logwrite(LOG_DEBUG, "fe: adapter %d frequency min %d max %d step %d tolerance %d",
				adapter->no,
				adapter->fe.feinfo.frequency_min,
				adapter->fe.feinfo.frequency_max,
				adapter->fe.feinfo.frequency_stepsize,
				adapter->fe.feinfo.frequency_tolerance);

	logwrite(LOG_DEBUG, "fe: adapter %d symbol rate min %d max %d tolerance %d",
				adapter->no,
				adapter->fe.feinfo.symbol_rate_min,
				adapter->fe.feinfo.symbol_rate_max,
				adapter->fe.feinfo.symbol_rate_tolerance);

#ifdef KLK_SOURCE
        return 0;        
#endif // KLK_SOURCE
}
#endif

int fe_tune_init(struct adapter_s *adapter) {
	char		fename[128];

	sprintf(fename, "/dev/dvb/adapter%d/frontend0", adapter->no);

	adapter->fe.fd=open(fename, O_RDWR|O_NONBLOCK);

	if (adapter->fe.fd < 0) {
		logwrite(LOG_ERROR, "Error opening dvb frontend %s", fename);
#ifdef KLK_SOURCE
                return -1;                
#else
		exit(-1);
#endif // KLK_SOURCE
	}

	logwrite(LOG_INFO, "fe: Adapter %d Setting up frontend tuner", adapter->no);

#ifdef KLK_SOURCE
        if (fe_checkcap(adapter) != 0)
            return -1;        
#else
	fe_checkcap(adapter);
#endif // KLK_SOURCE


	/* Single shot - try to tune */
#ifdef KLK_SOURCE
        if (fe_tune(adapter) != 0)            
            return -1;                
#else
	fe_tune(adapter);
#endif // KLK_SOURCE

	/* Watch the filedescriptor for frontend events */
	event_set(&adapter->fe.event, adapter->fe.fd, EV_READ|EV_PERSIST, fe_event, adapter);
#ifdef KLK_SOURCE
        event_base_set(adapter->klkbase, &adapter->fe.event);        
#endif // KLK_SOURCE
	event_add(&adapter->fe.event, NULL);

	/* Create a timer to regular poll the status */
	fe_timer_init(adapter);

	return 0;
}

#ifdef KLK_SOURCE
void fe_tune_deinit(struct adapter_s *adapter)
{
    event_del(&adapter->fe.event);    
    evtimer_del(&adapter->fe.timer);        
    if (adapter->fe.fd >= 0)
    {   
        close(adapter->fe.fd);
    }    
}
#endif
