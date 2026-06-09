# Task Tracker CLI

A lightweight command-line task management tool built in C that helps you track your tasks with persistent JSON storage.

## Features

* ✅ Add, update, and delete tasks
* 📋 List all tasks or filter by status
* 🔄 Mark tasks as `todo`, `in-progress`, or `done`
* 🕒 Automatic timestamp tracking (created & updated)
* 💾 Persistent storage in JSON format
* ⚡ Fast and lightweight — no external dependencies

## Installation

### Prerequisites

* GCC or any C compiler
* [cJSON](https://github.com/DaveGamble/cJSON) library

### Build from Source

1. Clone the repository:

```bash
git clone https://github.com/opu-hossain/tasktracker-cli.git
cd tasktracker-cli
```

2. Install cJSON (if not already installed):

```bash
# Ubuntu/Debian
sudo apt-get install libcjson-dev

# macOS
brew install cjson

# Or compile from source
git clone https://github.com/DaveGamble/cJSON.git
cd cJSON
mkdir build && cd build
cmake ..
make
sudo make install
```

3. Compile the project:

```bash
gcc -o task-cli task-cli.c -lcjson
```

4. (Optional) Add it to your PATH:

```bash
sudo cp task-cli /usr/local/bin/
```

## Usage

### Adding a Task

```bash
./task-cli add "Complete project documentation"
```

**Output:**

```text
Added task 1: Complete project documentation
```

### Listing Tasks

List all tasks:

```bash
./task-cli list
```

Filter by status:

```bash
./task-cli list done
./task-cli list todo
./task-cli list in-progress
```

### Updating a Task

```bash
./task-cli update 1 "Complete revised documentation"
```

### Marking Task Status

```bash
./task-cli mark-in-progress 1
./task-cli mark-done 1
```

### Deleting a Task

```bash
./task-cli delete 1
```

## Data Structure

Tasks are stored in `tasks.json` with the following structure:

```json
[
  {
    "id": 1,
    "description": "Eat dinner",
    "status": "done",
    "createdAt": 1780891833,
    "updatedAt": 1780923570
  }
]
```

## Properties

| Property      | Type   | Description                                    |
| ------------- | ------ | ---------------------------------------------- |
| `id`          | Number | Unique task identifier                         |
| `description` | String | Task description                               |
| `status`      | String | Current status (`todo`, `in-progress`, `done`) |
| `createdAt`   | Number | Unix timestamp of creation                     |
| `updatedAt`   | Number | Unix timestamp of last modification            |

## Project Structure

```text
tasktracker-cli/
├── task-cli.c      # Main source code
├── tasks.json      # Task storage file
└── README.md       # Project documentation
```

## Error Handling

The CLI provides clear error messages for common issues:

* Missing task descriptions
* Invalid status filters
* Non-existent task IDs
* File read/write failures
* Invalid command usage

## Technical Details

* **Language:** C
* **Dependencies:** cJSON (for JSON parsing)
* **Storage:** Local JSON file (`tasks.json`)
* **Timestamps:** Unix timestamps formatted as `YYYY-MM-DD HH:MM:SS`

## Contributing

Contributions are welcome! Here's how you can help:

1. Fork the repository.

2. Create a feature branch:

   ```bash
   git checkout -b feature/amazing-feature
   ```

3. Commit your changes:

   ```bash
   git commit -m "Add amazing feature"
   ```

4. Push to the branch:

   ```bash
   git push origin feature/amazing-feature
   ```

5. Open a Pull Request.

## License

This project is open source and available under the MIT License.

## Author

**Opu Hossain**

* GitHub: [@opu-hossain](https://github.com/opu-hossain)

## Acknowledgments

* cJSON for JSON parsing in C
* Inspired by the [roadmap.sh](https://roadmap.sh/) [Task Tracker](https://roadmap.sh/projects/task-tracker) project
