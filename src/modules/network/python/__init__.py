from net import *  
from defines import *  

import cli
import proto

""" Creates a module instance
    @param[in] server - the server instance
"""
def createModuleInstance(server):
    return Net(server)

def importCLI(parent):
    """ Import a CLI instance to main CLI
    """
    netcli = cli.CLI(parent)
    return netcli
