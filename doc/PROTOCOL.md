# ACP

arf chat protocol

## Header

```
  4 bytes
+-------------------------------+
| MAGIC NUMBER                  |
+-------+-------+-------+-------+
| TYPE  | FLAGS | DATA LENGTH   |
+-------+-------+---------------+
| SOURCE UID                    |
+-------------------------------+
| DATA                          |
| ...                           |

```

Magic number: 0x6942

## Packet types

### NOP  0x00
No operation. No data.

### PING 0x01
Autodiscovery request. No data.

### PONG 0x02
Autodiscovery reply.

Data:
```
+---------------+---------------+
| RID           |               |
+---------------+---------------+
| Nickname NUL-terminated       |
| ...                           |
+-------------------------------+
| Room name NUL-terminated      |
| ...                           |
```

### JOIN 0x03
Room join

Data:
```
+---------------+---------------+
| RID           |               |
+---------------+---------------+
| Room name NUL-terminated      |
| ...                           |
```

### RMSG 0x04
Room message

Data:
```
+---------------+---------------+
| RID           |               |
+---------------+---------------+
| Message NUL-terminated        |
| ...                           |
```


