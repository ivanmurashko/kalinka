from defines import *  
from factory import *  

import server

def importCLI(parent):
    """ Import a CLI instance to main CLI
    """
    cli = server.CLI(parent)
    return cli
