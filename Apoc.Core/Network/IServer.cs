using System;
using System.Collections.Generic;
using System.Text;
using System.Net;

namespace Apoc.Network
{
    /// <summary>
    /// A callback/delegate function for client connection event
    /// </summary>
    /// <param name="clientIP">The IP address of the client that got connected</param>
    public delegate void HandleClientConnection(String clientIP);

    /// <summary>
    /// A callback/delegate function for client disconnection event
    /// </summary>
    /// <param name="clientIP">The IP address of the client that got disconnected</param>
    public delegate void HandleClientDisconnection(String clientIP);

    /// <summary>
    /// An interface that defines the properties and methods of a network server.
    /// </summary>
    public interface IServer
    {
        /// <summary>
        /// Gets or sets the port number used to transfer messages.
        /// </summary>
        int PortNumber { get; set; }

        /// <summary>
        /// Gets the address of this server in 4 bytes.
        /// </summary>
        /// <remarks>
        /// It returns 4 bytes instead of String in order to optimize the network transfer.
        /// (e.g., 192.168.0.1 will be byte[0] = (byte)192, byte[1] = (byte)168, byte[2] = (byte)0,
        /// byte[3] = (byte)1)
        /// </remarks>
        byte[] MyIPAddress { get; }
        
        /// <summary>
        /// Gets the number of connected clients to this server.
        /// </summary>
        int NumConnectedClients { get; }

        /// <summary>
        /// Gets the addresses of connected clients.
        /// </summary>
        List<String> ClientIPAddresses { get; }

        /// <summary>
        /// Gets or sets whether to enable encryption.
        /// </summary>
        bool EnableEncryption { get; set; }

        /// <summary>
        /// Adds or removes an event handler for client connection event.
        /// </summary>
        event HandleClientConnection ClientConnected;

        /// <summary>
        /// Adds or removes an event handler for client disconnection event.
        /// </summary>
        event HandleClientDisconnection ClientDisconnected;

        /// <summary>
        /// Initializes this server for accepting connections.
        /// </summary>
        void Initialize();

        /// <summary>
        /// Broadcasts a message to all of the connected clients except the sender if 
        /// 'excludeSender' is set to false. Otherwise, it broadcasts to all connected clients.
        /// </summary>
        /// <param name="msg">The message to broadcast</param>
        /// <param name="reliable">Whether the message is guaranteed to arrive at the
        /// receiver side</param>
        /// <param name="inOrder">Whether the message arrives in order at the receiver side</param>
        /// <param name="excludeSender">Whether to exclude the sender</param>
        void BroadcastMessage(byte[] msg, bool reliable, bool inOrder, bool excludeSender);

        /// <summary>
        /// Sends a message to a list of specific clients with the given IP addresses.
        /// </summary>
        /// <param name="msg">The message to send</param>
        /// <param name="ipAddresses">The IP addresses of the clients to send to</param>
        /// <param name="reliable">Whether the message is guaranteed to arrive at the
        /// receiver side</param>
        /// <param name="inOrder">Whether the message arrives in order at the receiver side</param>
        void SendMessage(byte[] msg, List<String> ipAddresses, bool reliable, bool inOrder);

        /// <summary>
        /// Receives a list of messages in byte arrays.
        /// </summary>
        /// <returns>A list of received messages in array of bytes</returns>
        List<byte[]> ReceiveMessage();

        /// <summary>
        /// Shuts down the server.
        /// </summary>
        void Shutdown();
    }
}
