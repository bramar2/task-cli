# task-cli:  A CLI tool to manage your tasks
https://roadmap.sh/projects/task-tracker
## Code
The project uses CMake. There is practically no comments inside the project (bad practice), probably no one can understand this project (I'll probably forget after a week).
## Building
On root, run the following commands:
- `cmake -S . -B build/`
- `cmake --build build/`


The executable should be in build/task-cli.exe.

## Preview
```
Task CLI --- A CLI tool to manage all your tasks | github.com/bramar2/task-cli
| task-cli add "Task description"               - Add a task and returns the Task ID.
| task-cli update [ID] "New desc"               - Update task description.
| task-cli delete [ID]                          - Delete a task.
| task-cli mark [ID] [todo/in-progress/done]    - Mark task status.
| task-cli list                                 - Display a list of all tasks.
| task-cli list [todo/in-progress/done]         - Display a list of all tasks with the specified status.
| task-cli terminal                             - Toggles terminal mode for quick task managing.
| task-cli exit                                 - Exit.
| task-cli help                                 - Show this help menu.
```
![image](https://github.com/user-attachments/assets/32684c85-1ca5-4691-bcc8-b561a24ee187)
