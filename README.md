# Peer-to-Peer Group Based File Sharing System

![alt text](https://github.com/[Mayhem-29]/[Mini_Bit_Torrent]/blob/[main]/WhatsApp Image 2022-11-15 at 4.06.22 PM.jpeg?raw=true)

# Working

1. User should create an account and register with tracker.
2. Login using the user credentials.
3. Tracker maintains information of clients with their files(shared by client) to assist the clients for the communication between peers.
4. User can create Group and hence will become admin of that group.
5. User can fetch list of all Groups in server.
6. User can join/leave group.
7. Group admin can accept group join requests.
8. Share file across group: Shares the filename and SHA1 hash of the complete file as well as piecewise SHA1 with the tracker.
9. Fetch list of all sharable files in a Group.
10. Download:
    1. Retrieve peer information from tracker for the file.
    2. Download file from multiple peers (different pieces of file from different peers - ​piece selection algorithm​) simultaneously and all the files which client downloads will be shareable to other users in the same group. File integrity is ensured using SHA1 comparison.
11. Piece selection algorithm used: Selects random piece and then downloads it from a random peer having that piece.
12. Show downloads.
13. Stop sharing file.
14. Logout - stops sharing all files.
15. Whenever client logins, all previously shared files before logout should automatically be on sharing mode.

## Assumptions

1. Only one tracker is implemented and that tracker should always be online.
2. The peer can login from different IP addresses, but the details of his downloads/uploads will not be persistent across sessions.
3. SHA1 integrity checking doesn't work correctly for binary files, even though in most likelihood the file would have downloaded correctly.
4. File paths should be absolute.
