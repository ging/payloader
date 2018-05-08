#include "TcpServer.h"
 
namespace payloader { 
pthread_mutex_t mutex1;
void* thread_function01(void* new_connection) {

  const boost::system::error_code& error = boost::system::errc::make_error_code(errc::not_supported);
  TcpConnection::pointer connection = *((TcpConnection::pointer*) new_connection);
  size_t size = 12;
  printf("Dentro\n");
  mutex1 = connection->getMutex();
  pthread_mutex_trylock(&mutex1);
  printf("Mutex locked para ver si se inicia el streaming\n");
  RTSP_CMD_TYPES C = connection->getC();
  pthread_mutex_unlock(&mutex1);
  printf("Mutex unlocked para ver si se inicia el streaming\n");
  //Estos dos se crean en cada conexion y hablan entre ellos 
  if( C == RTSP_PLAY){ // hay que llamar aqui al metodo que hace una respuesta de tal forma que esquivamos la zona de exclusion mutua
    printf("enviando\n");
    payloader::Streamer* m_Streamer = connection->getStreamer();
    m_Streamer->StartStreaming();
  }else{
     printf("C es:%d\n",C);
  }
  return 0;
}


DEFINE_LOGGER(TcpServer, "TcpServer");
// Constructor: initialises an acceptor to listen on TCP port 8554.
TcpServer::TcpServer(boost::asio::io_service& io_service) : acceptor_(io_service, tcp::endpoint(tcp::v4(), 8554)){//Creamos el soocket en el puerto 8554 escuchando tcp para la conexión rtsp
  // start_accept() creates a socket and 
  // initiates an asynchronous accept operation 
  // to wait for a new connection.
  start_accept();
}
TcpServer::~TcpServer(){
    //io_service.stop();
}

void TcpServer::tomaPuerto(long int puerto){
      this->puerto = puerto;
}
long int TcpServer::getterData(){
      return puerto;
}
void TcpServer::start_accept(){
  // creates a socket
  //Hay que mandar esto a otra hebra para mantener la conexión tcp
 
  payloader::Streamer* m_Streamer = new payloader::Streamer();                                                                                             //Crear streamer O EN EL .H? PARA INVOCAR DESDE LA HEBRA
  //tcp_connection::pointer new_connection = tcp_connection::create(acceptor_.get_io_service(), m_Streamer); // pasarle el steamer
  payloader::TcpConnection* new_connection = new payloader::TcpConnection(acceptor_.get_io_service(), m_Streamer);
  TcpConnection::pointer new_connection_pointer = new_connection->create(acceptor_.get_io_service(), m_Streamer);//pero aqui que estamos creando¿¿¿¿?¿?
  // initiates an asynchronous accept operation 
  // to wait for a new connection. 
  acceptor_.async_accept(new_connection_pointer->socket(),//tiene que ser un puntero!!!!
      boost::bind(&TcpServer::handle_accept, this, new_connection_pointer,
        boost::asio::placeholders::error));

}
// handle_accept() is called when the asynchronous accept operation 
// initiated by start_accept() finishes. It services the client request
void TcpServer::handle_accept(TcpConnection::pointer new_connection, const boost::system::error_code& error){
  if (!error){   
    //Creo una hebra a la que le paso la conexion asociada a los datos que vienen por 8554, la conexion esta creada pero NO ejecutada.
    pthread_create( &mithread, NULL, &thread_function01,  &new_connection);//pasarle la conexión  
    // Call start_accept() to initiate the next accept operation.
  }
    start_accept();
};
  //Crear streamer 
    //new_connection->registerParent(this);
  /*puerto = new_connection->getPort();
        
      //struct param param1 = {puerto};
      printf("puerto es: %ld\n", puerto);*/
         
}  
