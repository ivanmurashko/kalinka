from defines import *  
from adapter import *  

""" Creates a module instance
    @param[in] server - the server instance
"""
def createModuleInstance(server):
    return Adapter(server)
