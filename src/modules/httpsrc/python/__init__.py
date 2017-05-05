from httpsrc import *  
from defines import *  

import cli

""" Creates a module instance
    @param[in] server - the server instance
"""
def createModuleInstance(server):
    return HTTPSource(server)

def importCLI(parent):
    """ Import a CLI instance to main CLI
    """
    httpsrccli = cli.CLI(parent)
    return httpsrccli
