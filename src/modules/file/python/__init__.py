from klkfile import *  
from defines import *  
import filetype

import cli

""" Creates a module instance
    @param[in] server - the server instance
"""
def createModuleInstance(server):
    return File(server)

def importCLI(parent):
    """ Import a CLI instance to main CLI
    """
    filecli = cli.CLI(parent)
    return filecli
