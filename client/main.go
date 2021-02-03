// Copyright (C) 2021 Quentin M. Kniep <hello@quentinkniep.com>
// Distributed under terms of the MIT license.

package main

import (
	"bufio"
	"fmt"
	"io"
	"net/http"
	"os"
	"strconv"
)

func main() {
}

func search(query string) {
}

func downloadToKindle(bookID int) {
	download(bookID, "/dev/Kindle...")
}

func download(bookID int, destination string) {
	f, err := os.Create(destination)
	defer f.Close()
	if err != nil {
		fmt.Println("Error: Failed to create file on Kindle.")
	}
	writer := bufio.NewWriter(f)
	resp, err := http.Get("localhost:9898/books/" + strconv.Itoa(bookID))
	if err != nil {
		fmt.Println("Error: Failed to contact server.")
	}
	io.Copy(writer, resp.Body)
}

// Uploads the eBook file to the server.
func upload(filename string) {
	f, err := os.Open(filename)
	defer f.Close()
	if err != nil {
		fmt.Println("Error: Failed to open eBook file.")
	}
	reader := bufio.NewReader(f)
	http.Post("localhost:9898/upload", "application/octet-stream", reader)
}
