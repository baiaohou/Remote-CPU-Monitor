# Remote CPU Monitor

### Author: Baiao Hou (implemented in C++, HTML, CSS, JavaScript)

Certain implementations of the UNIX operating system make information about the underlying hardware and the operating system itself available via files in the “/proc” directory. These are not actual files on the disk, but are rather like “device files” in the “/dev” directory in that they are a way of providing information using standard file I/O mechanisms. One such file is “/proc/stat”, which provides statistics about the amount of time the CPUs on the host machine spend doing various things like executing user processes, handling interaction with external devices, or doing nothing, which we call being “idle”.

## Overview

The goal of this project is to develop a Remote CPU Monitor which keeps track of how much time the CPUs on a host computer are spending executing processes. This would allow a system administrator to get information about how busy a computer’s CPUs are and to decide, for instance, if some processes need to be terminated or if additional machines/CPUs are needed in order to complete some task.

This program recorded the time of processes execution. Calculated current/average/maximum usages from remote access via HTTP requests. Plus, a multi-threaded web server with CPU statistics autoupdates is developed, providing with usage data in chart (D3.js library), and threshold alerts.

### Auto-Update

Instead of requiring the user to refresh the browser in order to get updated readings, it should be possible for the user to click a button in the HTML page that puts it into “auto-update mode” such that, once a second, it automatically makes an HTTP request to the server to get the latest data. It does not require reloading the entire HTML page, but rather only retrieving the data from the server and using JavaScript to update the HTML content. 

### Threshold Alert

The user should be able to specify a threshold by entering a number into an HTML input field and clicking a button, such that if the next CPU usage percentage that is displayed in the browser exceeds that threshold, the value is displayed in red and a JavaScript alert is shown informing the user that the value has exceeded the threshold; if the value does not exceed the threshold, then no alert should be shown. 

### Display Usage Data in a Chart

Used JavaScript library D3.js to produce a chart showing a graphical representation of the CPU usage data. The x-axis should represent time and the y-axis should show the usage percentage at that time. This is combined with the “Auto-Update” feature so that the chart automatically update itself.

### Changing Frequency

By default, the server reads /proc/stat once a second. It should be possible for the user to change the frequency with which the server reads /proc/stat by typing a number into an HTML field and clicking a button so that the number is sent to the server and is used to specify the amount of time that the server “sleeps” before re-reading /proc/stat. 

### Multi-threaded HTTP Server

The HTTP server that we provided runs in a single thread, so that if the server is handling an HTTP request and another comes in, the second has to wait until the first one finishes before it can be handled. Modify the “start_server” function so that, after accepting an incoming connection, it creates a new thread to handle the request and send back the response, so that the first thread can go back to waiting for incoming connections. 


