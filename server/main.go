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

	log.Println("[Info] Server now listening on localhost:9898")
	log.Fatal(http.ListenAndServe(":9898", nil))
}

func bookHandler(w http.ResponseWriter, r *http.Request) {
	var err error
	switch r.Method {
	case "GET":
		bookList(w, r)
	case "POST":
		log.Println("[Info] User is uploading a new eBook file")
		if err = uploadFile(w, r); err == nil {
			log.Println("[Info] Successfully uploaded File")
		}
	default:
		log.Println("[Warn] Received invalid request:", r)
		w.WriteHeader(http.StatusMethodNotAllowed)
	}
	if err != nil {
		log.Println("[Error]", err)
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

func uploadFile(w http.ResponseWriter, r *http.Request) error {
	// grab the file from the multipart form
	r.ParseMultipartForm(10 << 20) // use up to 10 MiB memory
	file, _, err := r.FormFile("ebook")
	if err != nil {
		return fmt.Errorf("retrieve file from form: %v", err)
	}
	defer file.Close()

	// store it in a temporary file
	// TODO (@qkniep): use the correct file extension
	// TODO (@qkniep): store it in a reasonable path
	filename := fmt.Sprintf("%v.pdf", len(books))
	tempFile, err := os.Create("/tmp/" + filename)
	if err != nil {
		return fmt.Errorf("create temporary file: %v", err)
	}
	defer tempFile.Close()
	if _, err := io.Copy(tempFile, file); err != nil {
		return fmt.Errorf("copy file contents: %v", err)
	}

	books = append(books, book{r.FormValue("title"), r.FormValue("author"), []string{"pdf", "epub"}})
	return nil
}
