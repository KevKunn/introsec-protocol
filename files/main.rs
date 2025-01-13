use std::env;

fn main() {
    const INTERNAL_PASSWORD: &str = "PASSWORD";

    let args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        eprintln!("Error: Expected one argument in the format 'username:password'.");
        std::process::exit(1);
    }

    let input = &args[1];
    match validate_and_split_input(input) {
        Ok((username, password)) => {
            let capitalized_username = capitalize(&username);
            let capitalized_password = capitalize(&password);

            if capitalized_password == INTERNAL_PASSWORD {
                println!("That was the correct password, {}!", capitalized_username);
            } else {
                println!("WRONG! That was totally wrong, {}!", capitalized_username);
            }
        }
        Err(e) => {
            eprintln!("Error: {}", e);
            std::process::exit(1);
        }
    }
}

fn validate_and_split_input(input: &str) -> Result<(String, String), &'static str> {
    if !input.contains(':') {
        return Err("Input must contain a colon separating the username and password.");
    }

    let parts: Vec<&str> = input.splitn(2, ':').collect();
    if parts.len() != 2 {
        return Err("Invalid input format.");
    }

    let username = parts[0];
    let password = parts[1];

    if username.is_empty() || password.is_empty() {
        return Err("Username and password cannot be empty.");
    }

    if username.len() > 10 || password.len() > 10 {
        return Err("Username and password must not exceed 10 characters.");
    }

    Ok((username.to_string(), password.to_string()))
}

fn capitalize(input: &str) -> String {
    input.to_uppercase()
}
