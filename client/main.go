// Copyright (C) 2021 Quentin M. Kniep <hello@quentinkniep.com>
// Distributed under terms of the MIT license.

package main

import (
	"bufio"
	"bytes"
	"flag"
	"fmt"
	"io"
	"io/ioutil"
	"mime/multipart"
	"net/http"
	"os"
)

func main() {
	searchCmd := flag.NewFlagSet("search", flag.ExitOnError)
	searchQuery := searchCmd.String("query", "", "Substring of the title")

	downloadCmd := flag.NewFlagSet("download", flag.ExitOnError)
	downloadPath := downloadCmd.String("path", "", "Where to store the downloaded eBook file")
	toKindle := downloadCmd.Bool("kindle", false, "Download directly to Kindle, if it is connected")

	uploadCmd := flag.NewFlagSet("upload", flag.ExitOnError)
	title := uploadCmd.String("title", "", "Path to the file you want to upload")
	author := uploadCmd.String("author", "", "Path to the file you want to upload")

	if len(os.Args) < 2 {
		fmt.Println("Expected one of 'search', 'download', or 'upload' as subcommand.")
		os.Exit(127)
	}

	var err error
	switch os.Args[1] {
	case "search":
		searchCmd.Parse(os.Args[2:])
		err = search(*searchQuery)
	case "download":
		downloadCmd.Parse(os.Args[2:])
		if *toKindle {
			err = downloadToKindle(0)
		} else {
			err = download(0, *downloadPath + "/test.pdf")
		}
	case "upload":
		uploadCmd.Parse(os.Args[2:])
		err = upload(uploadCmd.Arg(0), *title, *author)
	}
	if err != nil {
		fmt.Println(err)
	}
}

// Searches for books matching the query string.
func search(query string) error {
	resp, err := http.Get("http://localhost:9898/books?query=" + query)
	if err != nil {
		return fmt.Errorf("Failed to contact server: %v", err)
	}
	body, err := ioutil.ReadAll(resp.Body)
	if err == nil {
		fmt.Printf(string(body))
	}

	return err
}

// Downloads the book directly to the Kindle.
// Returns an error if no Kindle is connected.
func downloadToKindle(bookID int) error {
	kindleDir, err := os.Open("/Volumes/Kindle/documents")
	if err != nil {
		return fmt.Errorf("No Kindle connected")
	}
	defer kindleDir.Close()
	return download(bookID, kindleDir.Name() + "/test.pdf")
}

// Downloads the book and writes it to the file given as destination.
func download(bookID int, destination string) error {
	f, err := os.Create(destination)
	if err != nil {
		return err
	}
	defer f.Close()
	writer := bufio.NewWriter(f)

	url := fmt.Sprintf("http://localhost:9898/files/%d.pdf", bookID)
	resp, err := http.Get(url)
	if err == nil {
		_, err = io.Copy(writer, resp.Body)
	}

	return err
}

// Uploads the eBook file to the server.
func upload(filename, title, author string) error {
	file, err := os.Open(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	var b bytes.Buffer
	w := multipart.NewWriter(&b)
	w.WriteField("title", title)
	w.WriteField("author", author)
	ffw, _ := w.CreateFormFile("ebook", file.Name())
	io.Copy(ffw, file)
	if err := w.Close(); err != nil {
		return err
	}

	client := &http.Client{}
	res, err := client.Post("http://localhost:9898/books", w.FormDataContentType(), &b)
	if err == nil && res.StatusCode != http.StatusOK {
		err = fmt.Errorf("Bad response status %v", res.Status)
	}

	return err
}
