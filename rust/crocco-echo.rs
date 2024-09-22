use anyhow::Result;
use std::net::SocketAddr;
use tokio::io::{AsyncBufReadExt, AsyncWriteExt, BufReader};
use tokio::net::{TcpListener, TcpStream};

async fn echo(mut socket: TcpStream, addr: SocketAddr) -> Result<()> {
    println!("Incoming connection from: {}", addr);
    let (read, mut write) = socket.split();
    let mut read = BufReader::new(read);
    write.write_all(b"Welcome!").await?;
    loop {
        let mut line = String::new();
        let len = read.read_line(&mut line).await?;
        if len == 0 {
            println!("Connection closed");
            break;
        }

        if line.trim().to_lowercase() == "exit" {
            println!("Exiting");
            break;
        }

        write.write_all(line.as_bytes()).await?;
        println!("Line: {}", line);
    }

    Ok(())
}

#[tokio::main]
async fn main() -> Result<()> {
    let listener = TcpListener::bind("localhost:16000").await?;
    loop {
        let (mut stream, addr) = listener.accept().await?;
        tokio::spawn(async move { echo(stream, addr).await });
    }

    println!("Hello, world!");
}
