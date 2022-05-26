# Peer-to-Peer Group Based File Sharing System

### Prerequisites
Socket Programming, SHA1 hash, Multi-threading in C++
### Goal
In this assignment, you need to build a group based file sharing system where users can share, download files from the group they belong to. Download should be parallel with multiple​ pieces from multiple peers.
### Note:
- You have to divide the file into logical  “pieces”, wherein the size of each piece should be 512KB.
- SHA1 : Suppose the file size is 1024KB, then divide it into two pieces of 512KB each and take SHA1 hash of each part, assume that the hashes are HASH1 & HASH2 then the corresponding hash string would be H1H2 , where H1 & H2 are starting 20 characters of HASH1 & HASH2 respectively and hence H1H2 is 40 characters.
- Authentication for login needs to be done

### Architecture Overview:
The Following entities will be present in the network :<br/>
**1. Server(or Tracker)( 1 tracker system) :**<br/>
     Maintain information of clients with their files(shared by client) to assist the clients for the communication between peers<br/>
**2. Clients:**<br/>
**a.** User should create an account and register with tracker<br/>
**b.** Login using the user credentials<br/>
**c.** Create Group and hence will become owner of that group<br/>
**d.** Fetch list of all Groups in server<br/>
**e.** Request to Join Group<br/>
**f.** Leave Group<br/>
**g.** Accept Group join requests (if owner)<br/>
**h.** Share file across group: Share the filename and SHA1 hash of the complete file as well as piecewise SHA1 with the tracker<br/>
**i.** Fetch list of all sharable files in a Group<br/>
**j.** Download file<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;**i.** Retrieve peer information from tracker for the file<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;**ii.** Core Part: Download file from multiple peers (different pieces of file from
different peers -  piece selection algorithm ) simultaneously and all the files which client downloads will be shareable to other users in the same group.<br/>
**k.** Show downloads<br/>
**l.** Stop sharing file<br/>
**m.** Stop sharing all files(Logout)<br/>
