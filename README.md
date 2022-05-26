Mini Bit torrent
Architecture Overview:
The Following entities are present in the network :
1. Tracker:

a. Maintain information of clients with their files(shared by client) to assist the
clients for the communication between peers


2. Clients:

a. User should create an account and register with tracker

b. Login using the user credentials

c. Create Group and hence will become owner of that group


d. Fetch list of all Groups in server

e. Request to Join Group

f. Leave Group

g. Accept Group join requests (if owner)

h. Share file across group: Share the filename and SHA1 hash of the complete file
as well as piecewise SHA1 with the tracker

i. Fetch list of all sharable files in a Group

j. Download file

i. Retrieve peer information from tracker for the file

ii. Core Part: Download file from multiple peers (different pieces of file from
different peers - piece selection algorithm) simultaneously and all the
files which client downloads will be shareable to other users in the same
group. Ensure file integrity from SHA1 comparison

k. Show downloads

l. Stop sharing file

m. Stop sharing all files(Logout)

n. Whenever client logins, all previously shared files before logout should
automatically be on sharing mode

