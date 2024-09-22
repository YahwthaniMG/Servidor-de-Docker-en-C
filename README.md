# TCP and UDP Server for Unity Multiplayer Game using C and hosting servers on Docker

This repository contains the server-side implementation for a multiplayer game built in Unity. The server is implemented in C and consists of both TCP and UDP components.

## Project Structure

The project is organized into two main folders:

- TCP/: Contains the TCP server and client implementations
- UDP/: Contains the UDP server and client implementations

Each folder includes both the server and client code for its respective protocol.

## How It Was Built

The server was developed using C programming language to ensure high performance and low-level network control. The project utilizes socket programming to implement both TCP and UDP communication protocols.

### TCP Server
The TCP server handles reliable, connection-oriented communication. It's used for:
- Player authentication
- Game state synchronization
- Critical game events

### UDP Server
The UDP server manages fast, connectionless communication. It's primarily used for:
- Real-time player movement
- Non-critical, frequent updates

## Setup Docker and Instantiate the servers


1. Clone this repository:
   
   git clone https://github.com/YahwthaniMG/Servidor-de-Docker-en-C
   

 <!--- 2. Navigate to the desired server directory (TCP or UDP):
   
   cd TCP/server
   
   or
   
   cd UDP/server
   

3. Compile the server code:
   
   gcc -o server server.c
   

4. Run the server:
   
   ./server
   

Repeat steps 2-4 for both TCP and UDP servers if you need to run both simultaneously.

## Connecting to the Server

To connect your Unity game client to this server:

1. Ensure the server is running.
2. In your Unity project, use the appropriate networking library to connect to the server's IP address and port.
3. Implement the necessary client-side logic to handle TCP and UDP communication based on your game's requirements.

## Docker Support

This project includes Docker support for easy deployment. To use Docker:

1. Ensure Docker is installed on your system.
2. Navigate to the project root directory.
3. Build the Docker image:
   
   docker build -t game-server .
   
4. Run the Docker container:
   
   docker run -p [host_port]:[container_port] game-server
   

Replace [host_port] and [container_port] with the appropriate port numbers for your TCP and UDP servers. --->

## Contributors

This project was made possible through the collaborative efforts of the following individuals:

1. Brandon Avalos
   - GitHub: [@Branxz07](https://github.com/branxz07)

2. Yahwthani Morales
   - GitHub: [@YahwthaniMG](https://github.com/YahwthaniMG)

3. Sebastian Avilez
   - GitHub: [@SebastianAz](https://github.com/0247473)

We appreciate the hard work and dedication of each team member in bringing this project to life. Their diverse skills and perspectives were instrumental in creating this TCP and UDP server for our Unity multiplayer game.

## License

This software is released into the public domain. Anyone is free to use, modify, distribute, or sell it without restriction.

## Disclaimer

This software, a TCP and UDP server for a multiplayer game in Unity, is a collaborative project created by [Your Name], branxz, yhaw, and sebas. It is provided for free use under the following conditions:

1. FREE USE: This software is available for anyone to use, modify, distribute, or integrate into their own projects at no cost.

2. NO WARRANTY: The software is provided "as is," without warranty of any kind, express or implied. This includes, but is not limited to, warranties of merchantability, fitness for a particular purpose, and non-infringement.

3. NO LIABILITY: In no event shall the authors or copyright holders be liable for any claim, damages, or other liability, whether in an action of contract, tort, or otherwise, arising from, out of, or in connection with the software or the use or other dealings in the software.

4. USE AT YOUR OWN RISK: Any person, entity, or organization choosing to use this software does so at their own risk. The authors are not responsible for any damages, data loss, security issues, or other problems that may occur as a result of using this software.

5. NO SUPPORT OBLIGATION: The authors are under no obligation to provide support, maintenance, updates, or bug fixes for this software.

6. ACKNOWLEDGMENT: By using this software, you acknowledge that you have read this disclaimer, understand its contents, and agree to its terms.

This project is not affiliated with Unity Technologies or any other commercial entity. Any trademarks, service marks, product names, or named features are assumed to be the property of their respective owners and are used only for reference.

Remember: If you're not comfortable with these terms, please do not use this software.

---

## Related Project

This server implementation is designed to work with our Unity-based multiplayer game. For the client-side implementation and the game itself, please refer to our Unity project repository:

[Unity Multiplayer Game Project](https://github.com/branxz07/FPS-Multiplayer-Unity-and-Docker.git)

The Unity project contains:
- Client-side networking code
- Game logic and mechanics
- User interface and graphics

To fully utilize this server, make sure to set up and configure the Unity project according to its README instructions. The two projects are designed to work together to create a complete multiplayer gaming experience.

---

Thank you for your interest in our TCP and UDP server project for Unity multiplayer games. If you have any questions or issues, please open an issue in this repository or the Unity project repository as appropriate.
