// Copyright (C) 2021 Quentin M. Kniep <hello@quentinkniep.com>
// Distributed under terms of the MIT license.

package main

import (
	"fmt"
	"net/http"
	"io"
	"io/ioutil"
	"log"
)

const maxUploadFileSize = 50 << 20 // 50 MiB

func main() {
	helloHandler := func(w http.ResponseWriter, req *http.Request) {
		io.WriteString(w, "Hello, world!\n")
	}

	http.HandleFunc("/hello", helloHandler)
	http.HandleFunc("/upload", uploadFile)

	log.Println("INFO: Server now listening.")
	log.Fatal(http.ListenAndServe(":9898", nil))
}

func uploadFile(w http.ResponseWriter, r *http.Request) {
	if r.Method != "POST" {
		log.Println("WARN: The /upload endpoint only supports POST.")
		return
	}
	log.Println("INFO: User is uploading a new eBook file.")

	// grab the file from the multipart form
	r.ParseMultipartForm(maxUploadFileSize)
	file, handler, err := r.FormFile("ebook")
	if err != nil {
		log.Println("Error Retrieving the File:", err)
		return
	}
	defer file.Close()
	log.Println("Uploaded File:", handler.Filename)
	log.Println("File Size:", handler.Size)
	log.Println("MIME Header:", handler.Header)

	// store it in a temporary file
	tempFile, err := ioutil.TempFile("/tmp", "upload-*.pdf")
	if err != nil {
		fmt.Println(err)
	}
	defer tempFile.Close()
	io.Copy(tempFile, file)
	fmt.Fprintf(w, "Successfully Uploaded File\n")
}
