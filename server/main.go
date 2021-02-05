// Copyright (C) 2021 Quentin M. Kniep <hello@quentinkniep.com>
// Distributed under terms of the MIT license.

package main

import (
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
	"strings"
)

type book struct {
	title   string
	author  string
	formats []string
}

var books []book

func main() {
	helloHandler := func(w http.ResponseWriter, req *http.Request) {
		io.WriteString(w, "Hello, world!\n")
	}

	http.HandleFunc("/hello", helloHandler)
	http.HandleFunc("/books", bookHandler)
	http.Handle("/files/", http.StripPrefix("/files/", http.FileServer(http.Dir("/tmp"))))

	log.Println("[Info] Server now listening.")
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
	queryStr := r.URL.Query().Get("query")
	for i, book := range books {
		if queryStr != "" && !strings.Contains(book.title, queryStr) {
			continue
		}
		formatsStr := strings.Join(book.formats, ", ")
		fmt.Fprintf(w, "%v - %v (%s)\n", i, book.title, formatsStr)
	}
}

func uploadFile(w http.ResponseWriter, r *http.Request) {
	log.Println("[Info] User is uploading a new eBook file.")

	// grab the file from the multipart form
	r.ParseMultipartForm(10 << 20) // use up to 10 MiB memory
	file, handler, err := r.FormFile("ebook")
	if err != nil {
		log.Println("[Error] Failed to retrieve the file:", err)
		return
	}
	defer file.Close()

	// store it in a temporary file
	filename := fmt.Sprintf("%v.pdf", len(books))
	tempFile, err := os.Create("/tmp/" + filename)
	if err != nil {
		fmt.Println(err)
	}
	defer tempFile.Close()
	io.Copy(tempFile, file)

	books = append(books, book{handler.Filename, "Some Author", []string{"pdf", "epub"}})
	fmt.Fprintf(w, "[Info] Successfully Uploaded File\n")
}
