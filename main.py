import http.client

def send_post_request():
    # Indirizzo del server e porta
    server_address = 'localhost'
    server_port = 8080

    # Dati da inviare nel corpo della richiesta POST
    post_data = 'nomePc=IlMiPC'

    # Creazione della connessione HTTP
    connection = http.client.HTTPConnection(server_address, server_port)

    # Definizione dell'endpoint e dei dati della richiesta POST
    endpoint = '/wo'
    headers = {'Content-type': 'application/x-www-form-urlencoded'}

    try:
        # Invio della richiesta POST
        connection.request('POST', endpoint, body=post_data, headers=headers)

        # Lettura della risposta
        response = connection.getresponse()
        print("Risposta dal server:")
        print(response.status, response.reason)
        print(response.read().decode('utf-8'))

    finally:
        # Chiusura della connessione
        connection.close()

if __name__ == "__main__":
    send_post_request()
