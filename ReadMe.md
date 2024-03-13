# Uno Game

## Card Engine
<ul>
The Card Engine library was developed to separate the entire core of the game from the application that will utilize it. The focus here is to have the entire UNO game running directly within this DLL in a reactive manner. This means that the game will react when its functions are invoked, potentially returning additional events to the application consuming this library.

<li><h3>Cards</h3></li>
The structure for the playable cards has been abstracted using an interface called ICard, through which the entire system interacts exclusively. Cards possess attributes such as *Number*, *Color*, *Type*, and *ID*, along with comparators with other cards to streamline logic creation, such as "*sameNumber*," "*sameColor*," and "*sameType*."

A series of unit tests have been developed for the cards to ensure that comparisons are conducted accurately and across various scenarios.

<li><h3>Deck</h3></li>
A collection of cards is abstracted through the IDeck interface, where this class is responsible for organizing the cards. Within it, methods mimic the functionality of a deck, allowing for the removal or return of a card from the top, discarding a card onto the pile, shuffling, and viewing the top card. A series of unit tests have been developed to ensure that all these functions operate as expected, with some instrumented tests demonstrating the lifecycle of a deck from start to finish and simulating the discard pile.

You can use the deck with ver intuitive methods like “*deck->stack(card.get());*” to throw a card in the front of the deck or “*deck->enqueue(card.get());*” to add a card to the back the deck or “*deck->deenqueue(card.get());*” to get and remove a card from the back of the deck.

<ul>
<li><h4>JSON Lib</h4></li>
One of the classes that inherit from the IDeck interface is capable of initializing the deck from a JSON file, thereby facilitating the customization of game sessions and the creation of new cards. This feature allows for dynamic configuration of the deck composition, enabling flexibility in gameplay and the addition of new card types or attributes as needed.

It was used the library [nlohmann json](https://github.com/nlohmann/json) to read and deserialize the data.

The structure used for the json objects is like this
```
{
   "type": "skip",
   "number": -1,
   "color": "b",
   "repeat": 1
}
```

- type: is used to define which class will be instantiated.
- number: defines the number field on the instantiated class.
- color: defines the color char field on the instantiated class.
- repeat: used to indicate how many copies will be created when generating the deck.

<li><h4>Card Factory</h4></li>
This class was developed to streamline the instantiation of cards for decks and to ensure that each card possesses unique IDs. By encapsulating card creation within this class, the process becomes more efficient and less error-prone, while the enforcement of unique IDs maintains integrity within the deck, preventing duplication or ambiguity in card identification.
</ul>


<li><h3>Turn System</h3></li>
Responsible for managing the game flow, this class tracks the current turn and determines which player should play during that turn.

A series of instrumented tests were created for this class to simulate its functionalities, including the flow of turns among all players, ensuring that players take their turns correctly, and reversing the turn flow when necessary. These tests verify the correctness and reliability of the game flow management, ensuring smooth gameplay and adherence to the game rules.

<li><h3>Player</h3></li>
The class responsible for player information encompasses attributes such as name, ID, cards in hand, and additional details like whether Uno was yelled during the last turn. This class serves as a container for player-related data, facilitating the management and tracking of player state throughout the game. It allows for easy access to player information and ensures that pertinent details, such as card holdings and game actions, are readily available for game logic and presentation purposes.

<li><h3>Game State Manager</h3></li>
The gameStateManager class serves as the core orchestrator of the game's dynamics, overseeing essential tasks such as managing decks, defining player configurations, enforcing game rules, and orchestrating the turn-based system. Let's delve into its key components and functionalities.

The class inherits from the IStateManager interface, providing a blueprint for managing the game state. It relies on various other components such as decks, players, and the turn system to execute its functionalities effectively. Additionally, it interacts with an event bus for handling game events and notifications.

Here are the core functionalities:

<ul>
<li><b>Deck Management:</b> The class maintains instances of the main deck and discard deck, essential components for gameplay progression.
  It ensures that decks are initialized properly and that cards are distributed and managed throughout the game.
</li>
<li><b>Turn System:</b> It collaborates with the turn system to regulate player turns, ensuring fairness and adherence to game rules.

  Methods like startGame(), endTurn(), and skipTurn() are instrumental in managing the turn-based flow of the game.
</li>
<li><b>Player Interaction:</b> The class facilitates player actions such as drawing cards, playing cards, and declaring Uno.

  It validates player moves against game rules and ensures that actions are executed appropriately.
</li>
<li><b>Game State Management:</b> Methods like getState() and setState() handle the serialization and deserialization of the game state, allowing for persistence and resumption of gameplay.</li>
<li><b>Game Control:</b> It oversees the overall control flow of the game, including starting, ending, and monitoring the game's progress.

  The class is equipped with a suite of instrumented tests designed to validate its functionalities comprehensively. These tests simulate various game scenarios, ensuring that rules are enforced, player actions are handled correctly, and the game flow progresses smoothly from start to finish.
</li>
</ul>
  In summary, the Game State Manager class encapsulates the core logic and control mechanisms of the game, orchestrating interactions between players, decks, and the turn system. Through its comprehensive functionalities and robust testing infrastructure, it ensures a seamless and enjoyable gaming experience while adhering to established rules and conventions.

</ul>

## NetLib
   This library was created for connection and communication using the TCP protocol. It serves as a tool for establishing reliable and efficient communication channels between networked devices or applications, leveraging the capabilities of the TCP protocol for data transmission and reception. By abstracting the complexities of TCP communication into a reusable library, developers can streamline the process of implementing networked functionalities in their software solutions, facilitating seamless interaction and data exchange over TCP/IP networks.

<ul>
<li><h3>TCP</h3></li>
The Transmission Control Protocol (TCP) is one of the most widely used protocols on the Internet. It mandates a connection between client and server, ensuring that information is reliably transmitted and received between both parties. However, TCP is not always the preferred protocol for games due to its reliability features, which can sacrifice speed. Many types of games require a high rate of information exchange per second, making TCP less suitable.

In this project, data transmission occurs sporadically, typically during turn exchanges. This type of communication, where data loss can be detected and errors can be handled effectively, makes TCP the optimal choice. While TCP may not offer the fastest transmission speeds, its reliability and error detection mechanisms are well-suited for scenarios where occasional data loss can be tolerated, such as in turn-based games.

<li><h3>Multi-thread</h3></li>
Both the server and the client are designed to operate with multiple threads running simultaneously to receive or send events. This multi-threaded architecture necessitates certain treatments to ensure that there are no simultaneous alterations of values, which could lead to data corruption or inconsistency.

To address this concern, synchronization mechanisms such as locks, semaphores, or mutexes are commonly employed. These mechanisms help regulate access to shared resources or critical sections of code, preventing multiple threads from accessing them simultaneously. By acquiring locks or semaphore permits before accessing shared data and releasing them afterward, threads can coordinate their actions and avoid conflicts.

Additionally, careful design and implementation of thread-safe data structures and algorithms can further mitigate the risks associated with concurrent access. For instance, using atomic operations or thread-safe containers provided by programming libraries can help maintain data integrity in multi-threaded environments.

Overall, ensuring thread safety in a multi-threaded application involves a combination of proper synchronization techniques, thoughtful design choices, and rigorous testing to identify and resolve potential concurrency issues. By adhering to best practices in concurrent programming, developers can create robust and reliable systems capable of handling concurrent operations effectively.

<li><h3>Command Pattern</h3></li>
Both the client and the server have been refactored to perform their actions based on commands. This approach allows for the creation of a log detailing the events that occurred during a game session, thereby facilitating debugging sessions.

By structuring interactions around commands, each action taken by the client or the server becomes a discrete event that can be logged and analyzed. This logging capability provides valuable insight into the sequence of events that transpired during gameplay, making it easier to identify and diagnose issues or unexpected behaviors.

Moreover, having a detailed history of commands executed by the client and the server enhances the transparency and traceability of the system. Developers can track the flow of actions, pinpointing any discrepancies or errors that may have occurred during gameplay.

Overall, refactoring the client and server to operate using commands not only streamlines the implementation and execution of actions but also significantly improves the debugging and troubleshooting process by providing a comprehensive record of events during game sessions.
</ul>

## MultiplayerCardEngine
   The  Multiplayer Card Engine library implements CardEngine with wrappers to ensure functionality in multiplayer environments. The wrappers serve as intermediary layers between the CardEngine logic and the NetLib infrastructure, abstracting away the complexities of network communication and synchronization. They facilitate seamless integration of multiplayer capabilities into the CardEngine, enabling players to engage in multiplayer gameplay without directly interacting with networking protocols or implementation details. This class extends the functionality of the base gameStateManager to accommodate networked gameplay and communication between clients and servers. The key features and functionalities for this library are:

<ul>
<li><b>Client-Server Architecture:</b> the class integrates both client and server components necessary for multiplayer interactions.
   <ul>
   <li> It manages the communication between clients and servers, facilitating game synchronization and event handling across networked environments.</li>
   </ul>
</li>
<li><b>Network Communication:</b> utilizes the netcode::client and netcode::server components to establish and maintain network connections.
   <ul>
   <li>Implements methods for sending and receiving game state data, commands, and callbacks between clients and servers.</li>
   </ul>
</li>
<li><b>Game Setup and Management:</b> provides methods for setting up game sessions, initializing player configurations, and managing game state transitions.
   <ul>
   <li>Enables encryption and decryption of game settings to ensure secure communication between clients and servers.</li>
   </ul>
</li>
<li><b>Command Execution and Callbacks:</b> defines templates for executing game commands and adding callbacks for handling specific events or actions.
   <ul>
   <li>Supports both client-side and server-side command execution and callback registration, ensuring consistency and coherence in gameplay logic.</li>
   </ul>
</li>
<li><b>Room Management:</b> manages the concept of rooms, allowing players to join and participate in specific game instances.
   <ul>
   <li>Provides functionality for verifying player connections within a room and notifying clients when a game session starts.</li>
   </ul>
</li>
<li><b>Purpose and Advantages:</b> the primary purpose of the netGameStateManager class is to enable multiplayer functionality within the CardEngine while abstracting the complexities of network communication. By encapsulating client-server interactions and providing a unified interface for game management, it simplifies the development of multiplayer games and ensures consistent gameplay experiences across networked environments.
</li>

The Multiplayer Card Engine library plays a pivotal role in facilitating multiplayer gameplay by integrating network communication capabilities into the CardEngine framework. Its robust features, including command execution, callback handling, and room management, contribute to the seamless coordination of game sessions across distributed systems. Through its design and implementation, it empowers developers to create engaging and interactive multiplayer experiences while maintaining scalability, reliability, and security in networked environments.

</ul>

<ul>
<li><h3>Networking Game State Encryption and Decryption</h3></li>

The project required client-server synchronization, and the chosen solution involved generating a "state" for the current game. However, this posed several challenges, particularly regarding the volume of information, size considerations, and encryption requirements.

- **Quantity of Information:** the game comprises numerous variables, many of which could be inferred by the client upon receipt. To address this, only card IDs and player IDs with associated card IDs were transmitted instead of all card information. This reduced the amount of data transmitted and minimized network overhead.
- **Size Considerations:** originally, variables such as player and card IDs were stored as size\_t, a 64-bit unsigned integer, which consumed excessive bytes. To optimize, IDs were downsized to uint8 for cards (max 2^8-1 = 255, suitable for playable decks) and uint16 for players (max 2^16-1 = 65,535, ensuring scalability for multiple players and prolonged server lifespan).
- **Cryptography:** despite optimizations, aligning and transmitting these values still consumed significant data. Therefore, cryptographic techniques were applied to convert all data into a byte stream. As a result, a game with two players and a 120-card deck consumes approximately 160 bytes for transmission. This figure remains relatively consistent, even with additional players, as most data is related to card IDs, which don't increase with player count but may vary with larger decks.

By carefully managing data transmission, optimizing variable sizes, and applying encryption, the project effectively minimized bandwidth usage and ensured efficient client-server synchronization, enabling seamless multiplayer gameplay with minimal network overhead.

</ul>

## Game Console
   This project was created to integrate all the developed libraries and provide a user interface for interacting with the game.

   By consolidating the libraries, the project aims to offer a cohesive and seamless experience for users to engage with the game. The user interface serves as the gateway through which players can access and interact with various features and functionalities offered by the game engine and its associated components.

   Through the user interface, players can navigate menus, initiate game sessions, make gameplay decisions, and view game outcomes. It acts as the bridge between the underlying game logic and the user's actions and feedback.

   By centralizing the game's functionality within a user-friendly interface, the project aims to enhance the overall user experience, making it more intuitive and accessible to players of all levels. Whether it's navigating menus, customizing game settings, or participating in gameplay, the user interface serves as the primary point of interaction, ensuring a smooth and engaging gaming experience.

<ul>
<li><h3>Input Handler</h3></li>
Is responsible for capturing and delivering all user inputs within a frame using the "EventBus" system to inform other classes without creating extensive dependencies, it retrieves the standard input handle and configures the console mode accordingly, allowing for the capture of user inputs. The key components and functionality for this class are:

- **Input Processing:** has a method that captures user inputs by reading from the console input buffer.
  - It iterates through the input buffer, identifying and processing keyboard events such as key presses.
  - For each recognized input event, the corresponding event is fired using the EventBus, notifying other classes of the user action.
- **Event Firing:** Depending on the key pressed, specific events are fired using predefined event IDs in a header file name “eventIds.h” that maps macros to id’s such as INPUT\_LEFT, INPUT\_RIGHT, INPUT\_UP, INPUT\_DOWN, INPUT\_OK, and INPUT\_CANCEL.
  - Each event carries relevant input data, such as the character pressed or directional input.
- **Minimizing Dependencies:** The use of the EventBus system allows the inputHandler class to communicate user inputs to other classes without creating tight coupling or dependencies between them.
  - By decoupling input processing from event handling, the class promotes modularity and flexibility within the application architecture.

<li><h3>Renderer</h3></li>
The renderer class is designed to render all the information in the window using a coordinate system, simulating a more detailed rendering in the console environment. Here are the key features and functionalities:

**Console Rendering:** the class utilizes console output to render information within the window, providing a visual representation of data and elements.

It employs a coordinate system to organize and position elements within the console window, enabling detailed rendering.

**Element Management:** the class manages a collection of elements to be rendered in the window.

Elements can be added, retrieved, or removed dynamically during runtime, providing flexibility and modularity in rendering content.

**Rendering Control:** the draw() method orchestrates the rendering process, drawing all elements onto the console window based on their respective coordinates and attributes.

The rendering process is optimized to minimize flickering and ensure smooth visual updates.

**Buffer Management:** it maintains window buffers to store rendering data, allowing for efficient manipulation and updating of the rendered content.

The class employs double buffering techniques to prevent visual artifacts and enhance rendering performance.

**Window Control:** the class monitors changes in the console window size and adjusts rendering accordingly to ensure optimal display.

It provides methods to clear the screen, reset the rendering environment, and handle console window events gracefully.

**User Interaction:** the renderer class can respond to user input and trigger redraws or updates as necessary, providing an interactive rendering experience.

The renderer class serves as a crucial component for rendering graphical elements within the console environment. By managing elements, coordinating rendering processes, and optimizing display performance, it enables the effective presentation of information to users in a detailed and visually appealing manner. The class enhances the user experience by providing dynamic rendering capabilities and ensuring smooth interaction within the console window.

<ul>
<li><h4>Screen Elements</h4></li>
The element class provides a foundation for creating basic graphical elements to be rendered within the renderer. These elements can include text, shapes, and images, contributing to the visual representation of data within the application, its features are:

- **Element Attributes:** each element is characterized by its unique identifier (id), position (position), size (size), character for drawing (drawChar), and color (color).
  - These attributes define the appearance and behavior of the element within the rendering environment.
- **Drawing Functionality:** the class defines a pure virtual function draw() responsible for rendering the element onto the rendering buffer.
  - Subclasses of *element* must implement this method to specify how the element should be drawn based on its specific characteristics.
- **Element Manipulation:** methods are provided to set and retrieve the position, size, and color of the element, allowing for dynamic adjustment and customization during runtime.
  - The getId() method returns the unique identifier of the element, facilitating element management and identification within the renderer.
- **Extensibility:** the class is designed to serve as a base for creating diverse types of elements, ranging from simple text displays to complex graphical representations.
  - Subclasses can extend the functionality of the element class by implementing custom drawing logic tailored to specific types of elements.

The element class forms the building blocks for creating and rendering graphical elements within the renderer framework. By encapsulating essential attributes and drawing functionality, it enables the creation of versatile and customizable visual components that contribute to the overall user interface and experience. The extensible nature of the class allows for the seamless integration of various types of elements, enriching the visual presentation and interactivity of the application.
</ul>

<li><h3>Screens</h3></li>

Classes responsible for what will be drawn on the screen:
- ### MainMenu:
  - Responsible for displaying the main menu screen of the application.
  - It includes options for starting the game offline and online and accessing settings.
  - Handles user input for navigating menu options and triggering actions.
- ### SettingsMenu:
  - Manages the settings menu screen where users can customize various game options and preferences, like player hand size, random seed, and number of players.
- ### Game:
  - Represents the main gameplay screen where the core mechanics of the game are implemented and displayed.
  - Renders the game table with all the players with their respective names and hand cards count, the hand cards of the player, the game options, and the current player information.
  - Handles user input for controlling which card to play or option to interact with.
- ### GameOver:
  - Displays the game over screen when the game session ends, with the name of the winning player.
  - Goes back to the previous screen after player interaction.
  - Presents feedback to the player about their performance during the game session.
- ### ConnectToServer:
  - Handles the user interface for connecting to a game server.
  - Allows users to input server addresses.
  - Initiates the connection process and provides feedback on connection status.
- ### RoomCreation:
  - Manages the interface for creating or connecting to a game room in the connected server environment.
  - Handles the creation of the game room and transitions to the room waiting screen.
- ### RoomWaiting:
  - Displays the waiting room screen where players gather and wait for the game session to start.
  - Allows users to set up room parameters such as hand size and random seed.
  - Provide information about other players in the room, their status, and readiness.
</ul>

- ## Cheats
  - They can be used in the client during offline matches by hitting the ‘ key to open the debug console or on the server by just typing the cheat
    - **Win Cheat**: used both in the offline game and the online game to instantly win the game for the current player.
