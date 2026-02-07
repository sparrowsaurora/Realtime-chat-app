# Realtime chat app

_**Abstract:**_ A real time chat application to help me better understand networking.  
_**Purpose:**_ A terminal application that lets you communicate quickly with others on the same connection.

## Key features

- TCP base
- CLI based tooling
- Custom Logging tool
- client side customisation

## About the project

An in-terminal realtime chatroom app, with the option to send messages from the terminal or within the UI

<!-- ![Image of Chatroom UI](assets/) -->

## How to use

Firstly: run the command to join a server/room.  
this runs the daemon process.

```shell
rtc run --room <id>
```

you can now view the room via the following command  
_this looks at the stored room ID_

```shell
rtc view
```

then when you want to send a message,  
you can either send a message in the chatroom ui  
or by using the % symbol in your terminal like

```shell
% this is a test message
```

## Credits

This project uses **argparse** by **p-ranav**, licensed under the **MIT** license type

## Contact

\> [github](https://github.com/sparrowsaurora)  
\> [email](mailto:sparrows.au@gmail.com)

[(Back To Top)](#Realtime-chat-app)
