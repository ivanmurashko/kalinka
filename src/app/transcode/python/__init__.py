from transcode import *  
from defines import *
import sourcetype
import schedule
import videosize
import videoquality

""" Creates a module instance
    @param[in] server - the server instance
"""
def createModuleInstance(server):
    return Transcode(server)
