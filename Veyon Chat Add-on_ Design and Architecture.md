# Veyon Chat Add-on: Design and Architecture

**Author**: Manus AI
**Date**: 2025-09-30

## 1. Introduction

This document outlines the design and architecture for a new open-source chat add-on for Veyon, a classroom management software. The add-on will provide a comprehensive, real-time communication solution between the Veyon Master (teacher) and connected clients (students), offering a free alternative to the existing commercial chat add-on.

This document will serve as the primary technical blueprint for the development of the chat add-on, covering its features, architecture, data model, and implementation details.

## 2. Goals and Objectives

The primary goal of this project is to create a feature-rich, stable, and easy-to-use chat add-on for Veyon. The key objectives are:

- To provide seamless, real-time, bidirectional communication between the Veyon Master and clients.
- To implement all the core features requested by the user, including F10 hotkey activation, master name customization, and global messaging.
- To incorporate the selected enhanced features, such as timestamps, message status indicators, and private messaging.
- To ensure the add-on is easily installable as a separate plugin for Veyon 4.9.7.
- To create a high-quality, open-source alternative to the commercial Veyon Chat add-on.



## 3. Architecture

The chat add-on will be built upon Veyon's existing plugin and feature management system. It will consist of two main components: a **Master-side plugin** and a **Client-side service**.

### 3.1. Component Overview

- **ChatFeaturePlugin**: This will be the main plugin class, inheriting from `FeatureProviderInterface`. It will be responsible for registering the chat feature with Veyon, creating the chat window on the Master side, and handling communication with the clients.

- **ChatMasterWidget**: A Qt widget that will serve as the main chat interface for the Master. It will display the list of connected clients, the chat history for each client, and provide controls for sending messages, clearing chats, and broadcasting global messages.

- **ChatServiceClient**: A component running on each client machine, integrated with the Veyon service. It will be responsible for receiving messages from the Master, displaying the chat window to the student, and sending messages back to the Master.

- **ChatClientWidget**: A Qt widget for the client-side chat interface. It will display the chat history with the Master and allow the student to send messages.

### 3.2. Communication

Communication between the Master and clients will be handled using Veyon's `FeatureMessage` system. This system allows for sending arbitrary data between the Master and clients. We will define a set of custom message types for our chat functionality, such as:

- `ChatMessage`: For sending a standard chat message.
- `ChatStatusUpdate`: For sending typing/away status updates.
- `MessageAcknowledgement`: For read receipts.
- `GlobalBroadcast`: For sending a message to all clients.
- `ClearChat`: To instruct the client to clear its chat history.

All communication will be asynchronous to ensure the UI remains responsive.



## 4. Features

The chat add-on will implement the following features, categorized into core and enhanced functionality:

### 4.1. Core Features

- **F10 Hotkey Activation**: Both the Master and client chat windows can be opened by pressing the F10 key.
- **Master Name Customization**: The Master's name displayed in the chat can be customized.
- **Client Name Display**: The client's computer name will be used as their display name in the chat.
- **Bidirectional Communication**: Clients can initiate conversations with the Master, and the Master can reply.
- **Global Broadcast**: The Master can send a message to all connected clients simultaneously.
- **Chat Clearing**: The Master can clear the chat history for any client.

### 4.2. Enhanced Features

- **Timestamp Display**: Every message will have a timestamp indicating when it was sent.
- **Message Status Indicators**: The chat will show the status of each message (sent, delivered, read).
- **Private Messaging**: The Master can send private messages to individual clients.
- **Client Status Indicators**: The chat will show when a user is typing or away.
- **Message Priority Levels**: Messages can be marked with different priority levels (e.g., Normal, Urgent).
- **Quick Reply Templates**: The Master will have a set of pre-defined templates for quick replies.
- **Customizable Chat Window**: The size and position of the chat window can be customized and saved.
- **Sound Notifications**: Users will receive sound notifications for new messages, which can be configured.
- **Keyboard Shortcuts**: In addition to the F10 hotkey, other keyboard shortcuts will be available for common actions.
- **Minimize to System Tray**: The chat window can be minimized to the system tray to keep it running in the background.



## 5. Data Model

The following data structures will be used to represent the chat data:

### 5.1. ChatMessage

This structure will represent a single chat message.

| Field       | Type   | Description                                      |
|-------------|--------|--------------------------------------------------|
| `senderId`    | String | Unique ID of the sender (Master or client)       |
| `receiverId`  | String | Unique ID of the receiver (Master or client)     |
| `messageId`   | String | Unique identifier for the message                |
| `timestamp`   | long   | The time the message was sent                    |
| `content`     | String | The text of the message                          |
| `priority`    | Enum   | `Normal`, `Urgent`, `Announcement`               |
| `status`      | Enum   | `Sent`, `Delivered`, `Read`                        |

### 5.2. ChatSession

This structure will represent a chat session with a single client.

| Field     | Type                | Description                                      |
|-----------|---------------------|--------------------------------------------------|
| `clientId`  | String              | The unique ID of the client                      |
| `history`   | List<ChatMessage>   | The chat history for this session                |

### 5.3. ClientStatus

This structure will represent the status of a client.

| Field    | Type   | Description                                      |
|----------|--------|--------------------------------------------------|
| `clientId` | String | The unique ID of the client                      |
| `status`   | Enum   | `Online`, `Away`, `Typing`                         |


_


## 6. UI/UX Design

The user interface will be designed to be intuitive and easy to use, following the existing Veyon design language. As per the user's preference, we will use Qt's traditional UI (.ui files) for simplicity and ease of development.

### 6.1. Master Chat Window

The Master chat window will have a two-panel layout:

- **Left Panel**: A list of all connected clients, with their computer names and current status (online, away, typing). The Master can select a client from this list to view their chat history.
- **Right Panel**: The chat history for the selected client. A text input field at the bottom will allow the Master to type and send messages. Buttons for sending messages, clearing the chat, and broadcasting a global message will be available.

### 6.2. Client Chat Window

The client chat window will be simpler, with a single panel showing the chat history with the Master. A text input field at the bottom will allow the student to type and send messages.

### 6.3. Notifications

- New messages will trigger a sound notification and a visual cue (e.g., a blinking icon in the system tray).
- The chat window will automatically scroll to the latest message.

