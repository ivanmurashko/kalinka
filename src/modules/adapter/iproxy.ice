/**
   @file iproxy.ice
   @brief This file is part of Kalinka mediaserver.
   @author ipp <ivan.murashko@gmail.com>

   Copyright (c) 2007-2010 Kalinka Team

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   CHANGE HISTORY

   @date
   - 2010/04/17 created by ipp (Ivan Murashko)
*/ 

#ifndef KLK_IPROXY_ICE
#define KLK_IPROXY_ICE

module klk
{
    module adapter
    {
        module ipc
        {
            /** @defgroup  IPC for adapter module
                
                Inter process communication structures and interfaces for 
                adapter module

                @ingroup grAdapterModule
                @{
            */

            /**
               List with data
            */
            sequence<string> StringList;

            /**
               Map string -> string

               There is a container for simple values
            */
            dictionary<string, string> SimpleDataStorage;
            
            /**
               Map string -> string list

               There is a container for values that are lists
            */
            dictionary<string, StringList> ListDataStorage;

            /**
               The structure keeps all info about a message
            */
            struct SMessage
            {
                /**
                   Message id
                */
                string mID; 

                /**
                   Sender module id
                */
                string mSender; 

                /**
                   Storage with simple values: string(key) -> string(value)
                */
                SimpleDataStorage mValues; 

                /**
                   Storage for list values: string(key) -> string list (value)
                */
                ListDataStorage mLists;

                /**
                   List with the message receivers (modules id)
                */
                StringList mReceivers;
            };            

            /**
               Proxy interface that allows send async and sync messages
            */
            interface IMessagesProxy
            {
                /**
                   Sends an async message

                   @param[in] msg - the message to be sent
                */
                void sendASync(SMessage msg);

                /**
                   Sends a sync message (message with response)

                   @param[in] msg - the message to be sent

                   @return the message with response 
                */
                SMessage sendSync(SMessage msg);     

                /**
                   Creates a message

                   @param[in] id - the message id to be created
                */
                SMessage getMessage(string id);                

                /**
                   Register a message
                   
                   @param[in] msgid - the message's id to be registered
                   @param[in] modid - the module's id that do the registration
                   @param[in] msgtype - the message's type                   
                */
                void registerMessage(string msgid, string modid, int msgtype);                
            };            

            /**
               Proxy interface that provides an access to 
               klk::IResources
            */
            interface IResourcesProxy
            {
                /**
                   Check is there the specified dev type
                   
                   @param[in] type - the dev's type to be checked
                */
                bool hasDev(string type);

                /**
                   Gets the resource id by its name

                   @param[in] name - the resource's name
                   
                   @return the dev uuid
                */
                string getResourceByName(string name);

                /**
                   Gets a list of devices ids by it's type

                   @param[in] type - the dev's type
                   
                   @return the list
                */
                StringList getResourceByType(string type);
            };            

            /**
               Proxy interface that provides an access to klk::IDev
            */
            interface IDevProxy
            {
                /**
                   Retrives the dev state

                   @param[in] uuid - the dev uuid

                   @return the state of the device
                */
                int getState(string uuid);                

                /**
                   Sets device state

                   @param[in] uuid - the dev uuid

                   @param[in] state - the state value to be set
                */
                void setState(string uuid, int state);

                /**
                   Tests is there a specified parameter available or not

                   @param[in] uuid - the dev uuid
                   @param[in] key - the key

                   @return
                   - true the parametr with the specified key exists
                   - false the parametr with the specified key does not exist
                */
                bool hasParam(string uuid, string key);

                /**
                   Gets dev info by it's key

                   @param[in] uuid - the dev uuid
                   @param[in] key - the key

                   @return the parameter value

                   @exception @ref klk::Exception
                */
                string getStringParam(string uuid, string key);                

                /**
                   Gets dev info by it's key

                   @param[in] uuid - the dev uuid
                   @param[in] key - the key

                   @return the parameter value

                   @exception @ref klk::Exception
                */
                int getIntParam(string uuid, string key);                

                /**
                   Sets a value for a dev's parameter

                   @param[in] uuid - the dev uuid
                   @param[in] key - the key for retriiving
                   @param[in] value - the value to be set
                   @param[in] mutableflag - do update (false) the last or not (true)

                   @see IDev::getIntParam
                   @see IDev::getStringParam
                */
                void setStringParam(string uuid, string key, string value, bool mutableflag);

                /**
                   Sets a value for a dev's parameter

                   @param[in] uuid - the dev uuid
                   @param[in] key - the key for retriiving
                   @param[in] value - the value to be set
                   @param[in] mutableflag - do update (false) the last or not (true)

                   @see IDev::getIntParam
                   @see IDev::getStringParam
                */
                void setIntParam(string uuid, string key, int value, bool mutableflag);

                /**
                   Gets last update time

                   @param[in] uuid - the dev uuid

                   @return the time
                */
                int getLastUpdateTime(string uuid);
            }; 

            /**
               Proxy interface that provides an access to 
               klk::IModuleFactory            
            */
            interface IModulesProxy
            {
                /**
                   Loads a module by it's id

                   @param[in] id - the module id to be loaded
                */
                void load(string id);
                
                /**
                   Unloads a module by it's id

                   @param[in] id - the module id to be unloaded
                */
                void unload(string id);       

                /**
                   Checks is the module loaded or not

                   @param[in] id - the module id to be checked

                   @return 
                   - true - the module has been loaded
                   - false - the module has not been loaded yet
                */
                bool isLoaded(string id);                
            };

            /** @} */
        };        
    };    
};

#endif //KLK_IPROXY_ICE
