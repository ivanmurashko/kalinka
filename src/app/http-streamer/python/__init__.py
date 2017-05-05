from httpstreamer import *  
from defines import *

import cli

""" Creates a module instance
    @param[in] server - the server instance
"""
def createModuleInstance(server):
    return HTTPStreamer(server)

def importCLI(parent):
    """ Import a CLI instance to main CLI
    """
    streamercli = cli.CLI(parent)
    return streamercli
