using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.Network
{
    /// <summary>
    ///  服务器连接事件的委派类型
    ///  A callback/delegate function for server connection event
    /// </summary>
    public delegate void HandleServerConnection();

    /// <summary>
    ///  服务器断开连接事件的委派类型
    ///  A callback/delegate function for server disconnection event
    /// </summary>
    public delegate void HandleServerDisconnection();

    /// <summary>
    ///  表示客户端
    ///  An interface that defines the properties and methods of a network client.
    /// </summary>
    public interface IClient
    {
        /// <summary>
        ///  获取用来传输数据的端口号
        ///  Gets the port number used to transfer messages.
        /// </summary>
        int PortNumber { get;}

        /// <summary>
        ///  获取该客户端的IP地址
        ///  Gets the address of this client in 4 bytes.
        /// </summary>
        /// <remarks>
        /// It returns 4 bytes instead of String in order to optimize network transfer.
        /// (e.g., 192.168.0.1 will be byte[0] = (byte)192, byte[1] = (byte)168, byte[2] = (byte)0,
        /// byte[3] = (byte)1)
        /// </remarks>
        byte[] MyIPAddress { get; }

        /// <summary>
        ///  获取或设置是否需要启用传输加密
        ///  Gets or sets whether to enable encryption.
        /// </summary>
        bool EnableEncryption { get; set; }

        /// <summary>
        ///  客户端连接到服务器的事件
        ///  Adds or removes an event handler for client connection event.
        /// </summary>
        event HandleServerConnection ServerConnected;

        /// <summary>
        ///  客户端从服务器断开的事件
        ///  Adds or removes an event handler for client disconnection event.
        /// </summary>
        event HandleServerDisconnection ServerDisconnected;

        /// <summary>
        ///  获取该客户端是否已经链接到了服务器
        ///  Gets whether this client is connected to a server.
        /// </summary>
        bool IsConnected { get; }

        /// <summary>
        ///  
        ///  Gets or sets whether to wait for the server to start up in case the server is 
        ///  not running when client tried to connect. To set the timeout for connection
        ///  trial, set the ConnectionTrialTimeOut property. Default value is false.
        /// </summary>
        /// <see cref="ConnectionTrialTimeOut"/>
        bool WaitForServer { get; set; }

        /// <summary>
        ///  
        ///  Gets or sets the timeout in milliseconds for connection trial when the server was
        ///  not up when client was started. This property is only effective if WaitForServer
        ///  is set to true. Default value is -1 which means the client waits for infinite time. 
        /// </summary>
        /// <see cref="WaitForServer"/>
        int ConnectionTrialTimeOut { get; set; }

        /// <summary>
        ///  通过主机名连接到服务器
        ///  Connects to the server specified by HostName.
        /// </summary>
        void Connect();

        /// <summary>
        ///  从服务器接收消息
        ///  Receives messages from the server.
        /// </summary>
        /// <returns>A list of received messages in array of bytes</returns>
        List<byte[]> ReceiveMessage();

        /// <summary>
        ///  向服务器发送消息
        ///  Sends a message to the server.
        /// </summary>
        /// <param name="msg">The message to be sent</param>
        /// <param name="reliable">Whether the message is guaranteed to arrive at the
        /// receiver side</param>
        /// <param name="inOrder">Whether the message arrives in order at the receiver side</param>
        void SendMessage(byte[] msg, bool reliable, bool inOrder);

        /// <summary>
        ///  关闭客户端
        ///  Shuts down the client.
        /// </summary>
        void Shutdown();
    }
}
