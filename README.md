# 🚀 Easy Crossplatform Drive (ECD)

Easy Crossplatform Drive is a robust client-server solution designed for fast and secure file sharing within local or global networks. The project features a centralized or private server deployment capability, making it suitable for both individual users and corporate environments.

---

## ⚠️ Project Status: Work In Progress (WIP)
**Note:** This project is currently in the **active development phase**. 
It is **not yet fully functional** or ready for production use.

---

## ✨ Core Features
* **Secure Authentication**: User registration with server-side password validation and email verification via 6-digit access codes.
* **Token-Based Sessions**: Secure HTTP interaction using unique Access Tokens generated upon login.
* **Advanced File Management**: Supports directory trees, file uploading/downloading, renaming, moving, and deletion.
* **Chunked Data Transfer**: Large files are handled via sequential chunk uploading and partial content downloading with Range header support.
* **Dynamic Updates**: Real-time file list updates using a Short Polling mechanism.
* **Detailed Logging**: Automated server-side logging of all incoming HTTP requests and responses.

---

## 🛠 Tech Stack
* **Language:** C++
* **Framework:** [Qt 6](https://www.qt.io/)
* **Build System:** CMake
* **Communication**: HTTP (JSON format for metadata, Octet-stream for binaries)
* **Database**: SQLite
* **Security:** [libsodium](https://doc.libsodium.org/)

---

## 📂 Database Tables
- ```users```: Stores unique credentials, emails, and secure password hashes.
- ```files```: Maintains metadata, ownership, and logical directory structures.
- ```tokens```: Manages hashed authorization tokens for secure session handling.

---

## 🚀 Quick Start

### 1. Clone the repository
To download the project along with all its dependencies, use the following command:

```bash
git clone --recurse-submodules https://github.com/dima-vs/Easy-Crossplatform-Drive.git
```

---

## 🗄️ Database Schema
### 1. `users` table
Stores credentials and basic information for all registered accounts.

| Field | Type | Constraints | Description |
| :--- | :--- | :--- | :--- |
| **id** | INTEGER | PRIMARY KEY, AUTOINCREMENT | Unique user identifier. |
| **username** | TEXT | UNIQUE, NOT NULL | Unique login name. |
| **email** | TEXT | UNIQUE, NOT NULL | User's email address. |
| **password_hash** | TEXT | NOT NULL | Securely hashed password. |

### 2. `files` table
Maintains metadata for every file and directory uploaded to the server.

| Field | Type | Constraints | Description |
| :--- | :--- | :--- | :--- |
| **id** | INTEGER | PRIMARY KEY, AUTOINCREMENT | Unique object identifier. |
| **owner_id** | INTEGER | NOT NULL, FK -> `users.id` | Reference to the file owner. |
| **type** | TEXT | NOT NULL | Type of object (`file` or `directory`). |
| **logical_name** | TEXT | NOT NULL | Filename as seen by the user. |
| **server_name** | TEXT | UNIQUE | Physical filename in the storage. |
| **size** | INTEGER | NOT NULL | File size in bytes. |
| **upload_time** | DATETIME | DEFAULT CURRENT_TIMESTAMP | Timestamp of upload. |
| **parent_id** | INTEGER | FK -> `files.id` | ID of the parent directory (NULL for root). |

>**Note:** `server_name` can be `NULL` only if the object is a directory.

### 3. `tokens` table
Manages hashed authorization tokens to handle secure user sessions.

| Field | Type | Constraints | Description |
| :--- | :--- | :--- | :--- |
| **id** | TEXT | PRIMARY KEY | Unique token ID. |
| **token_hash** | TEXT | NOT NULL | Hashed token for identity verification. |
| **user_id** | INTEGER | NOT NULL, FK -> `users.id` | Associated user identifier. |
| **expires_at** | DATETIME | NOT NULL | Token expiration timestamp. |