from defines import *  
from service import *  

""" Creates a module instance
    @param[in] server - the server instance
"""
def createModuleInstance(server):
    return Service(server)
