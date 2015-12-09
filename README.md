# Archives

    CAUTION: NOT MAINTAINED. Code and technologies might be outdated.

A collection of few projects from my college days. Feel free to use as a reference, primarily for academic purposes.

## SIP Security Testing Framework

A complete security testing framework for the Session Initiation Protocol (SIP) stack.
For more details refer to this paper: 
  - http://ieeexplore.ieee.org/xpl/login.jsp?tp=&arnumber=4784778

## Secure Unix file system

An encrypted Unix style file system with implementation of several commands such as open, close, seek, chmod, ls, cd, mkdir and more. 

## Simple P2P (Torrent) file system

A peer-to-peer (P2P) system consisting of a set of nodes that act as a distributed repository of files allowing participating nodes to search for and download files from each other.

## Implementation of a Wireless Ad-Hoc Protocols

Simulation of two wireless ad-hoc protocols on a wired network to understand the behavior of wireless broadcast and compare the power consumption of the two protocols. The implementation is based on the following papers:
  - http://ieeexplore.ieee.org/xpl/login.jsp?tp=&arnumber=1209241
  - http://ieeexplore.ieee.org/xpl/login.jsp?tp=&arnumber=4025074

## Vector Clocks Simulation

Simulation of Vector clocks using a network of distributed systems.
  - https://en.wikipedia.org/wiki/Vector_clock

## Constraint based network topology designer

Given, the location of N nodes in a plane by their coordinates, create a network topology graph such that it has the following constraints:
  - It contains all the given nodes.
  - It forms a connected graph.
  - Each node is connected to at least 2 other nodes (that is, the degree of each vertex in the graph is at least 2).
  - The diameter of the graph is at most 4. By this we mean that any node can be reached from any other node in at most 4 hops.
  - The total cost of the network topology is as low as possible, where the cost is measured by the total geometric length of all links.

The goal is to implement an optimization algorithm for this specific problem.

## Network threat sniffer

A network packet sniffer using the Linux pcap library that allows the admin to monitor all file transfers in the network. It alerts the admin if any suspicious files (with predefined words) are being transferred, providing details such as the source IP, destination IP, time of transfer, logged in users and even a copy of the file.

## Linux text editor

A full fledged text editor using the Linux ncurses library.
  - https://en.wikipedia.org/wiki/Ncurses

## Windows paint tool

A full fledged paint too using the Windows 2D graphics.h library.
  - https://en.wikipedia.org/wiki/Borland_Graphics_Interface

**Enjoy!**

