#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>

#define SERVER  "/tmp/serversocket"
#define MAXMSG  512

int make_named_socket(const char *filename); 

int main (void)
{
  int sock;
  char message[MAXMSG];
  struct sockaddr_in name;
  socklen_t size;
  int nbytes; 

  /* Remove the filename first, it's ok if the call fails */
  unlink (SERVER);

  /* Make the socket, then loop endlessly. */
  sock = make_named_socket (SERVER);
  while (1)
    {
      /* Wait for a datagram. */
      size = sizeof (name);
      nbytes = recvfrom (sock, message, MAXMSG, 0, (struct sockaddr *) &name, &size);
      if (nbytes < 0)
        {
          perror ("recfrom (server)");
          exit (EXIT_FAILURE);
        }

      /* Give a diagnostic message. */
      fprintf (stderr, "Server: got message: %s\n", message);

      /* Bounce the message back to the sender. */
      nbytes = sendto (sock, message, nbytes, 0, (struct sockaddr *) & name, size);
      if (nbytes < 0)
        {
          perror ("sendto (server)");
          exit (EXIT_FAILURE);
        }
    }
}

int make_named_socket (const char *filename)
{
  struct sockaddr_in name;
  int sock;
  size_t size;

  /* Create the socket. */
  sock = socket (AF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

  /* Bind a name to the socket. */
   /* Address family = Internet */
  name.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  name.sin_port = htons(7891);
  /* Set IP address to localhost */
  name.sin_addr.s_addr = htonl(INADDR_ANY);
  /* Set all bits of the padding field to 0 */
  memset(name.sin_zero, '\0', sizeof name.sin_zero);

  if (bind (sock, (struct sockaddr *) &name, sizeof(name)) < 0)
    {
      perror ("bind");
      exit (EXIT_FAILURE);
    }

  return sock;
}
