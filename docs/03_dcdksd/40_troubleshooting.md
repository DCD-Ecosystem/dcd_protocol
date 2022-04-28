---
content_title: dcdksd Troubleshooting
---

## How to solve the error "Failed to lock access to wallet directory; is another `dcdksd` running"?

Since `dcdcli` may auto-launch an instance of `dcdksd`, it is possible to end up with multiple instances of `dcdksd` running. That can cause unexpected behavior or the error message above.

To fix this issue, you can terminate all running `dcdksd` instances and restart `dcdksd`. The following command will find and terminate all instances of `dcdksd` running on the system:

```sh
pkill dcdksd
```
