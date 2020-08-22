package main

import "C"

import (
	"context"
	"errors"
	"fmt"
	"net/http"
	"os"
	"strings"
	"time"

	client "github.com/kkdai/youtube/v2"
)

func getBestFormat(video *client.Video) (client.Format, error) {
	m := make(map[string][]client.Format)
	m["hd2160"] = make([]client.Format, 0)
	m["hd1440"] = make([]client.Format, 0)
	m["hd1080"] = make([]client.Format, 0)
	m["hd720"] = make([]client.Format, 0)
	m["large"] = make([]client.Format, 0)
	m["medium"] = make([]client.Format, 0)
	m["small"] = make([]client.Format, 0)
	m["tiny"] = make([]client.Format, 0)
	for _, itag := range video.Formats {
		if len(strings.Split(itag.MimeType, "video/mp4")) != 1 { //Only consider mp4
			continue
		}

		if _, ok := m[itag.Quality]; ok {
			m[itag.Quality] = append(m[itag.Quality], itag)
		}
	}

	if len(m["hd2160"]) > 0 {
		return m["hd2160"][0], nil
	}
	if len(m["hd1440"]) > 0 {
		return m["hd1440"][0], nil
	}
	if len(m["hd1080"]) > 0 {
		return m["hd1080"][0], nil
	}
	if len(m["hd720"]) > 0 {
		return m["hd720"][0], nil
	}
	if len(m["large"]) > 0 {
		return m["large"][0], nil
	}
	if len(m["medium"]) > 0 {
		return m["medium"][0], nil
	}
	if len(m["small"]) > 0 {
		return m["small"][0], nil
	}
	if len(m["tiny"]) > 0 {
		return m["tiny"][0], nil
	}
	return client.Format{}, errors.New("No video found")

}

//export getVideoUrl
func getVideoUrl(videoLink string) *C.char {
	cl := client.Client{}
	fmt.Println(videoLink)

	httpTransport := &http.Transport{
		IdleConnTimeout:       60 * time.Second,
		TLSHandshakeTimeout:   10 * time.Second,
		ExpectContinueTimeout: 1 * time.Second,
	}

	cl.HTTPClient = &http.Client{Transport: httpTransport}
	video, err := cl.GetVideo(videoLink)
	if err != nil {
		return C.CString("")
	}

	format, err := getBestFormat(video)

	if err != nil {
		return C.CString("")
	}

	url, err := cl.GetStreamURL(context.Background(), video, &format)

	if err != nil {
		return C.CString("")
	}

	return C.CString(url)
}

func main() {
	str := getVideoUrl(os.Args[1])
	fmt.Println(str)
}
