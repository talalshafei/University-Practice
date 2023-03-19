# Overview

This task aims to help you practice the AVL Tree ADT. The program will 
be used to analyse songs available in the given playlist. this application is to index the 
songs and find out the songs belonging to the most popular artist and genre in a given song 
playlist.

A text file called “songs.txt” that includes the data about the 
songs in the playlist.

The task here is to process this file, and generate an AVL tree based on the artist name data. If
there is more than one post with the same artist name, they should then be stored together in 
the same node. Based on this data representation, the program must provides
the following functionalities to the user:
1. Display the full index: This will display the full AVL tree constructed. For traversal, the songs will be displayed alphabetically sorted based on their artist name.
2. The artist with the maximum number of songs: This will display the details (Song Name, Song 
Genre, Album Name, Artist Name, Release Year and Date) of the songs of artist which has 
maximum number of songs.
3. The oldest song: This will display the details (Song Name, Song Genre, Album Name, Artist 
Name, Release Year and Date) of the oldest song (i.e. song with minimum Release Year).
