// Copyright (C) 2021 Quentin M. Kniep <hello@quentinkniep.com>
// Distributed under terms of the MIT license.

package main

import (
	"fmt"
	"net/http"
	"io"
	"log"
	"os"
)

const maxUploadFileSize = 50 << 20 // 50 MiB

var books []string

func main() {
	helloHandler := func(w http.ResponseWriter, req *http.Request) {
		io.WriteString(w, "Hello, world!\n")
	}

	http.HandleFunc("/hello", helloHandler)
	http.HandleFunc("/books", bookHandler)

	log.Println("INFO: Server now listening.")
	log.Fatal(http.ListenAndServe(":9898", nil))
}

func bookHandler(w http.ResponseWriter, r *http.Request) {
	switch r.Method {
	case "GET":
		bookList(w, r)
	case "POST":
		uploadFile(w, r)
	default:
		w.WriteHeader(http.StatusMethodNotAllowed)
	}
}

func bookList(w http.ResponseWriter, r *http.Request) {
	for i, book := range books {
		fmt.Fprintf(w, "%v - %v\n", i, book)
	}
}

func uploadFile(w http.ResponseWriter, r *http.Request) {
	log.Println("[Info] User is uploading a new eBook file.")

	// grab the file from the multipart form
	r.ParseMultipartForm(maxUploadFileSize)
	file, handler, err := r.FormFile("ebook")
	if err != nil {
		log.Println("[Error] Failed to retrieve the file:", err)
		return
	}
	defer file.Close()
	log.Println("Uploaded File:", handler.Filename)
	log.Println("File Size:", handler.Size)
	log.Println("MIME Header:", handler.Header)

	// store it in a temporary file
	filename := fmt.Sprintf("%v.pdf", len(books))
	tempFile, err := os.Create("/tmp/" + filename)
	if err != nil {
		fmt.Println(err)
	}
	defer tempFile.Close()
	io.Copy(tempFile, file)

	books = append(books, handler.Filename)
	fmt.Fprintf(w, "[Info] Successfully Uploaded File\n")
}
