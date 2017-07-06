/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2015, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/ 
/* <DESC>
 * simple HTTP POST using the easy interface
 * </DESC>
 */ 
#include <stdio.h>
#include <curl/curl.h>
 
int main(void)
{
  CURL *curl;
  CURLcode res;
 
  /* In windows, this will init the winsock stuff */ 
  curl_global_init(CURL_GLOBAL_ALL);
 
  /* get a curl handle */ 
  curl = curl_easy_init();
  if(curl) {
    /* First set the URL that is about to receive our POST. This URL can
       just as well be a https:// URL if that is what should receive the
       data. */ 
    /* POST��󂯎��URL��ݒ肵�܂� */
    curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.0.6/tinyRetail_index/get_post.php");
    /* Now specify the POST data */ 
    /* POST�f�[�^��w�肷�� */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "name=daniel&project=curl");

    fprintf(stdout,"test0\n");    
 
    /* Perform the request, res will get the return code */ 
    /* �v������s����ƁAres�͖߂�R�[�h��擾���܂� */
    res = curl_easy_perform(curl);

    fprintf(stdout,"test1\n");
    
    /* Check for errors */ 
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    fprintf(stdout,"test2\n");
 
    /* always cleanup */ 
    /* ��ɃN���[���A�b�v */
    curl_easy_cleanup(curl);

    fprintf(stdout,"test3\n");
    
  }
  curl_global_cleanup();
  return 0;
}