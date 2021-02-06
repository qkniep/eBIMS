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
)

func main() {
	upload("/Users/qkniep/Downloads/Armada.pdf")
	search("Arm")
	//download(0, "/tmp/test.pdf")
	err := downloadToKindle(0)
	if err != nil {
		fmt.Println(err.Error())
	}
}

// Searches for books matching the query string.
func search(query string) error {
	resp, err := http.Get("http://localhost:9898/books?query=" + query)
	if err != nil {
		return fmt.Errorf("[Error] Failed to contact server: %v", err)
	}
	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		return fmt.Errorf("[Error] Failed to read response body: %v", err)
	}
	fmt.Println(string(body))

	return nil
}

func downloadToKindle(bookID int) error {
	kindleDir, err := os.Open("/Volumes/Kindle/documents")
	if err != nil {
		return fmt.Errorf("[Error] no Kindle connected")
	}
	return download(bookID, kindleDir.Name() + "/test.pdf")
}

// Downloads the book ...
func download(bookID int, destination string) error {
	f, err := os.Create(destination)
	if err != nil {
		return fmt.Errorf("[Error] failed to create file: %v", err)
	}
	defer f.Close()
	writer := bufio.NewWriter(f)

	url := fmt.Sprintf("http://localhost:9898/files/%d.pdf", bookID)
	resp, err := http.Get(url)
	if err != nil {
		return fmt.Errorf("[Error] failed to contact server: %v", err)
	}
	io.Copy(writer, resp.Body)

	return nil
}

// Uploads the eBook file to the server.
func upload(filename string) error {
	f, err := os.Open(filename)
	if err != nil {
		return fmt.Errorf("[Error] failed to open eBook file: %v", err)
	}
	defer f.Close()

	var b bytes.Buffer
	w := multipart.NewWriter(&b)
	fw, err := w.CreateFormFile("ebook", f.Name())
	if err != nil {
		return fmt.Errorf("[Error] failed to create form file: %v", err)
	}
	if _, err = io.Copy(fw, f); err != nil {
		return fmt.Errorf("[Error] failed to copy file contents: %v", err)
	}
	w.Close()

	req, err := http.NewRequest("POST", "http://localhost:9898/books", &b)
	if err != nil {
		return fmt.Errorf("[Error] failed to create request: %v", err)
	}
	req.Header.Set("Content-Type", w.FormDataContentType())

	client := &http.Client{}
	res, err := client.Do(req)
	if err != nil {
		return fmt.Errorf("[Error] failed to contact server: %v", err)
	}

	if res.StatusCode != http.StatusOK {
		return fmt.Errorf("[Error] bad status: %v", res.Status)
	}

	return nil
}
