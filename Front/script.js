

// what side is the player
const params = new URLSearchParams(window.location.search);
const side = params.get('side');
const game = new Chess();
var whiteSquareGrey = '#a9a9a9'
var blackSquareGrey = '#696969'
var player = '';

const board = Chessboard('board', {
    draggable: true,
    position: 'start',
    dropOffBoard: 'snapback',
    onDragStart: onDragStart,
    onDrop: handleMove,
    onMouseoutSquare: onMouseoutSquare,
    onMouseoverSquare: onMouseoverSquare,
    orientation: side === 'black' ? 'black' : 'white',
});

// connection to server
const socket = new WebSocket('ws://localhost:8080');


function removeGreySquares () {
    $('#board .square-55d63').css('background', '')
}

function onMouseoutSquare() {
    removeGreySquares();
}

function greySquare (square) {
    var $square = $('#board .square-' + square);
  
    var background = whiteSquareGrey;
    if ($square.hasClass('black-3c85d')) {
      background = blackSquareGrey
    }
  
    $square.css('background', background)
}

function onMouseoverSquare(square, piece) {
    var moves = game.moves({ square: square, verbose: true});

    if (moves.length === 0) return;

    if ((game.turn() === 'w' && player === 'black') || (game.turn() === 'b' && player === 'white')) return;

    greySquare(square);

    for (var i = 0; i < moves.length; i++) {
        greySquare(moves[i].to);
    }
}

function onDragStart(source, piece, position, orientation) {
    if (game.game_over()) return false;

    if ((game.turn() === 'w' && piece.startsWith('b')) || (game.turn() === 'b' && piece.startsWith('w'))
    || (game.turn() === 'w' && player === 'black') || (game.turn() === 'b' && player === 'white')) {
        return false;
    }

    return true;
}

function handleMove(source, target) {
    const legalMoves = game.moves({ square: source, verbose: true });

    const isLegalMove = legalMoves.some((move) => move.to === target);

    // illegal move
    if (!isLegalMove) {
        return 'snapback';
    }

    const promotionNeeded = legalMoves.some(
        (move) => move.to === target && move.promotion
    );

    if (promotionNeeded) {
        // Show promotion modal
        showPromotionModal(source, target);
        return; // Wait for user input
    }

    // player's move
    const move = game.move({ from: source, to: target }); 

    if ( move.flags === 'k' || move.flags === 'q' || move.flags === 'e'){
        board.position(game.fen());
    }
    
    if (!move) return 'snapback';

    if(game.game_over()){
        handleGameOver()
        return;
    }

    // send the updated FEN to the server
    sendFenToServer(game.fen());
}

function handleGameOver() {
    setTimeout(() => {
        setTimeout(() => {

            socket.send("END_GAME");
            socket.close();
            // Smoothly fade out the board
            const boardElement = document.getElementById('board');
            boardElement.classList.add('hidden'); // Add fade-out effect

            boardElement.style.display = 'none'; // Hide after fade-out

            // Show the winner modal after the board disappears
            const modal = document.createElement('div');
            modal.className =
                'fixed inset-0 flex items-center justify-center bg-black bg-opacity-50';

            const winnerText = game.in_checkmate()
                ? `Game Over! ${game.turn() === 'w' ? 'Black' : 'White'} wins by checkmate!`
                : game.in_stalemate()
                ? 'Game Over! It\'s a stalemate!'
                : game.in_draw()
                ? 'Game Over! It\'s a draw!'
                : 'Game Over!';

            modal.innerHTML = `
                <div class="bg-white p-6 rounded-lg shadow-lg text-center">
                    <h2 class="text-xl font-bold mb-4">${winnerText}</h2>
                    <button id="returnButton" class="px-8 py-4 bg-gradient-to-r from-blue-500 to-indigo-600 text-white text-lg font-bold rounded-lg shadow-lg hover:from-blue-600 hover:to-indigo-700 transform hover:scale-105 transition">
                        Return to Home
                    </button>
                </div>
                `;

            document.body.appendChild(modal);

            // Redirect to index.html on button click
            document.getElementById('returnButton').onclick = () => {
                window.location.href = 'index.html';
            };
        }, 1000);
    }, 500); // Match the fade-out duration
}



// Show Promotion Modal
function showPromotionModal(source, target) {
    const modal = document.getElementById('promotionModal');
    modal.classList.remove('hidden');


    const pieceColor = game.turn() === 'w' ? 'white' : 'black';
    // Add click listeners to promotion buttons
    const buttons = document.querySelectorAll('.promotion-button');
    const icons = {
        white: { q: '♕', r: '♖', b: '♗', n: '♘' },
        black: { q: '♛', r: '♜', b: '♝', n: '♞' },
    };
    buttons.forEach((button) => {
        const pieceType = button.getAttribute('data-piece');
        button.textContent = icons[pieceColor][pieceType]; // Set the correct icon
    });
    buttons.forEach((button) => {
        button.onclick = () => {
            const promotionPiece = button.getAttribute('data-piece');

            // Perform the promotion
            const move = game.move({
                from: source,
                to: target,
                promotion: promotionPiece,
            });

            // Hide the modal
            modal.classList.add('hidden');

            // Update the board
            board.position(game.fen());

            // Send the updated FEN to the server
            sendFenToServer(game.fen());
            
        };
    });
}

////////// Communication with the server!! //////////////////////////


window.onbeforeunload = () => {
    socket.send("END_GAME");
    socket.close();
};

socket.onmessage = (event) => {
    try {
        // Parse the move directly as a string
        const message = event.data.trim().replace(/\x00/g, '');
        if(message === "AI_CREATED") {
            console.log("created");
            return;
        }else if(message === "AI_DELETED") {
            console.log("deleted");
            return;
        }
        else if (message.length >= 4 && message.length <= 5) {
            const from = message.slice(0, 2); // First two characters
            const to = message.slice(2, 4);   // Next two characters
            const promotion = message.length === 5 ? message[4] : undefined; // Optional promotion character

            // Apply the move to the game logic
            game.move({ from, to, promotion });
            board.position(game.fen());
        } else {
            console.error("Invalid move format received from server:", message);
        }
    } catch (error) {
        console.error("Error processing server response:", error);
    }

    if (game.game_over()) {
        handleGameOver();
        return;
    }
    
};

socket.onopen = () =>{
    console.log('Connected to server');
    player = 'white'

    socket.send("START_GAME");

    if (side === 'black'){
        player = 'black'
        sendFenToServer(game.fen());
    }
}

socket.onerror = (error) => {
    console.error("WebSocket error:", error);
};

socket.onclose = (event) => {
    console.log("WebSocket connection closed:", event);
};

function sendFenToServer(fen) {
    if (socket.readyState === WebSocket.OPEN) {
        socket.send(fen);
    } else {
        console.error("WebSocket is not open. Cannot send FEN.");
    }
}