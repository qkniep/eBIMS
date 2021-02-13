// Copyright (C) 2021 Quentin M. Kniep <hello@quentinkniep.com>
// Distributed under terms of the MIT license.

package main

import (
	"bufio"
	"encoding/gob"
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
	"strings"
)

const (
	address = "localhost:9898"
	booksDir = "Books"
)

type book struct {
	Title   string
	Author  string
	Formats []string
}

var books []book

func main() {
	load(&books, "books.gob")
	log.Printf("[Info] Loaded %v book entries from disk", len(books))

	http.HandleFunc("/books", bookHandler)
	http.Handle("/files/", http.StripPrefix("/files/", http.FileServer(http.Dir(booksDir))))

	log.Println("[Info] Server now listening on", address)
	log.Fatal(http.ListenAndServe(address, nil))
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
		if queryStr != "" && !strings.Contains(book.Title, queryStr) {
			continue
		}
		formatsStr := strings.Join(book.Formats, ", ")
		fmt.Fprintf(w, "%v - %v [%s]\n", i, book.Title, formatsStr)
	}
}

func uploadFile(w http.ResponseWriter, r *http.Request) error {
	// grab the file from the multipart form
	r.ParseMultipartForm(10 << 20) // use up to 10 MiB memory
	memFile, _, err := r.FormFile("ebook")
	if err != nil {
		return fmt.Errorf("retrieve form file: %v", err)
	}
	defer memFile.Close()

	// store it in a temporary file
	// TODO (@qkniep): use the correct file extension
	// TODO (@qkniep): store it in a reasonable path
	filePath := booksDir + fmt.Sprintf("/%v.pdf", len(books))
	permanentFile, err := os.Create(filePath)
	if err != nil {
		return fmt.Errorf("create file %v on disk: %v", filePath, err)
	}
	defer permanentFile.Close()
	if _, err := io.Copy(permanentFile, memFile); err != nil {
		return fmt.Errorf("copy file contents: %v", err)
	}

	// add an entry into our list of books
	books = append(books, book{r.FormValue("title"), r.FormValue("author"), []string{"pdf"}})
	dump(books, "books.gob")
	return nil
}

func dump(obj interface{}, filename string) {
	f, err := os.Create(booksDir + "/" + filename)
	if err != nil {
		return
	}
	defer f.Close()
	w := bufio.NewWriter(f)
	enc := gob.NewEncoder(w)
	enc.Encode(obj)
	if err := w.Flush(); err != nil {
		log.Printf("dump %v to disk: %v\n", filename, err)
	}
}

func load(obj interface{}, filename string) {
	f, err := os.Open(booksDir + "/" + filename)
	if err != nil {
		return
	}
	defer f.Close()
	r := bufio.NewReader(f)
	dec := gob.NewDecoder(r)
	if err := dec.Decode(obj); err != nil {
		log.Printf("load %v from disk: %v\n", filename, err)
	}
}
