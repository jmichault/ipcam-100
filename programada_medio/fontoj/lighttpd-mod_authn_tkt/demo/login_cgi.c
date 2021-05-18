#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/md5.h>

#ifndef MD5_DIGEST_LENGTH
#define MD5_DIGEST_LENGTH 16
#endif

#define CONST_BUF_LEN(x) ((x) ? (x) : NULL), strlen(x)

typedef struct authn_tkt_struct {
    char * uid;
    char * tokens;
    char * user_data;
    char const * addr_buf;
    unsigned char digest[MD5_DIGEST_LENGTH];
} authn_tkt;

static void ticket_digest_MD5(authn_tkt *parsed, time_t timestamp, const char *secret)/*{{{*/
{
    uint32_t ts = htonl((uint32_t)timestamp); /*(use low 32-bits from time_t)*/
    MD5_CTX ctx;

    /* Generate the initial digest */
    MD5_Init(&ctx);
    if (NULL != parsed->addr_buf) {
        MD5_Update(&ctx,(unsigned char *)CONST_BUF_LEN(parsed->addr_buf));
    }
    MD5_Update(&ctx, (unsigned char *)&ts, sizeof(ts));
    MD5_Update(&ctx, (unsigned char *)CONST_BUF_LEN(secret));
    MD5_Update(&ctx, (unsigned char *)CONST_BUF_LEN(parsed->uid));
    if (parsed->tokens != NULL) {
        MD5_Update(&ctx, (unsigned char *)CONST_BUF_LEN(parsed->tokens));
    }
    if (parsed->user_data != NULL) {
        MD5_Update(&ctx,(unsigned char*)CONST_BUF_LEN(parsed->user_data));
    }

    MD5_Final(parsed->digest, &ctx);

    /* Generate the second digest */
    MD5_Init(&ctx);
    MD5_Update(&ctx, parsed->digest, MD5_DIGEST_LENGTH);
    MD5_Update(&ctx, (unsigned char *)CONST_BUF_LEN(secret));
    MD5_Final(parsed->digest, &ctx);
}


static int base64_encode(unsigned char const * const input, size_t const input_length, char * const output, size_t const max_output_length)
{
    char const encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                   'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                   'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                   'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                   'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                   'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                   'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                   '4', '5', '6', '7', '8', '9', '+', '/'};
    int const mod_table[] = {0, 2, 1};
    size_t const output_len = 4 * ((input_length + 2) / 3);

    if (output_len > max_output_length) {
        return -1;
    }

    for (int i = 0, j = 0; i < input_length;) {

        uint32_t octet_a = i < input_length ? (unsigned char)input[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)input[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)input[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        output[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        output[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        output[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        output[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[input_length % 3]; i++) {
        output[output_len - 1 - i] = '=';
    }
    output[output_len] = '\0';

    return 0;
}

int main(int argc, char * argv[])
{
    char token[1024];
    char base64_encoded[256];
    char const * secret = "abcdefghijklmnopqrstuvwxyz0123456789";
    char const * ip_addr = getenv("REMOTE_ADDR");
    authn_tkt tkt = {
        .uid = "UserID",
        .tokens = "",
        .user_data = "HOLA",
        .addr_buf = ip_addr,
    };
    time_t timestamp = time(NULL);
    ticket_digest_MD5(&tkt, timestamp, secret);
    char digest_hexstr[MD5_DIGEST_LENGTH * 2 + 1];
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(digest_hexstr + 2 * i, "%02x", tkt.digest[i]);
    }
    digest_hexstr[MD5_DIGEST_LENGTH * 2] = '\0';

    sprintf(token, "%s%08lx%s!%s!%s", digest_hexstr, timestamp, tkt.uid, tkt.tokens, tkt.user_data);
    base64_encode(token, strlen(token), base64_encoded, 256);
    printf("Status: 200 OK\r\n");
    printf("Content-type: text/html\r\n");
    printf("Set-Cookie: auth_tkt=%s\r\n\r\n", base64_encoded);
    puts("<!doctype html>");
    puts("<html lang=\"en\">");
    puts("<head>");
    puts("<meta charset=\"utf-8\">");
    puts("<title>My Auto Login CGI</title>");
    puts("</head>");
    puts("<body>");
    puts("<p>You have been logged in! Try accessing the <a href=\"/protected.html\">Secrets</a> now<p>");
    puts("<p>This <b>C</b> CGI should check username and password in whichever way and then generate the cookie, but for this example it automatically lets you in<p>");
    printf("<p>The auth_tkt Cookie should look like: %s<p>\n", base64_encoded);
    puts("<p>Press F12 to check it out in the dev tools of your browser<p>");
    puts("<p>If you modify or delete it, you'd be logged off<p>");
    puts("</body>");
    return 0;
}
