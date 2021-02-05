// Copyright (C) 2021 Quentin M. Kniep <hello@quentinkniep.com>
// Distributed under terms of the MIT license.

package main

import (
	"bufio"
	"bytes"
	"fmt"
	"io"
	"io/ioutil"
	"mime/multipart"
	"net/http"
	"os"
	"strconv"
)

func main() {
	upload("/Users/qkniep/Downloads/Armada.pdf")
	fmt.Println("TEST:")
	search("Test")
	//download(0, "/tmp/test.pdf")
}

// Searches for books matching the query string.
func search(query string) {
	resp, err := http.Get("http://localhost:9898/books?query=" + query)
	//resp, err := http.Get("http://localhost:9898/books")
	if err != nil {
		fmt.Println("Error: Failed to contact server.")
		return
	}
	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		fmt.Println("Error: Failed to read response body.")
		return
	}
	fmt.Println(string(body))
}

func downloadToKindle(bookID int) {
	download(bookID, "/dev/Kindle...")
}

// Downloads the book ...
func download(bookID int, destination string) {
	f, err := os.Create(destination)
	if err != nil {
		fmt.Println("Error: Failed to create file on Kindle.")
	}
	defer f.Close()
	writer := bufio.NewWriter(f)
	resp, err := http.Get("localhost:9898/books/" + strconv.Itoa(bookID))
	if err != nil {
		fmt.Println("Error: Failed to contact server.")
	}
	io.Copy(writer, resp.Body)
}

// Uploads the eBook file to the server.
func upload(filename string) error {
	f, err := os.Open(filename)
	if err != nil {
		return fmt.Errorf("[Error] Failed to open eBook file: %v", err)
	}
	defer f.Close()

	var b bytes.Buffer
	w := multipart.NewWriter(&b)
	fw, err := w.CreateFormFile("ebook", f.Name())
	if err != nil {
		return fmt.Errorf("[Error] Failed to create form file: %v", err)
	}
	if _, err = io.Copy(fw, f); err != nil {
		return fmt.Errorf("[Error] Failed to copy file contents: %v", err)
	}
	w.Close()

	req, err := http.NewRequest("POST", "http://localhost:9898/books", &b)
	if err != nil {
		return fmt.Errorf("[Error] Failed to create request: %v", err)
	}
	req.Header.Set("Content-Type", w.FormDataContentType())

	client := &http.Client{}
	res, err := client.Do(req)
	if err != nil {
		return fmt.Errorf("[Error] Failed to contact server: %v", err)
	}

	if res.StatusCode != http.StatusOK {
		return fmt.Errorf("[Error] Bas status: %v", res.Status)
	}

	return nil
}
