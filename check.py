def check_threat(direction, opponent_pieces, threataning_pieces, my_king_position, own_pieces_positions):
        for i in range(1, 8):
            king_new_position = my_king_position[0] + direction[0] * i, my_king_position[1] + direction[1] * i
            if king_new_position in own_pieces_positions:
                return False
            
            for piece in opponent_pieces:
                if piece.position == king_new_position:
                    if type(piece) in threataning_pieces:
                        return True
                    else:
                        return False
            

def am_i_in_check(my_king_position, own_pieces_positions, opponent_pieces, whose_turn):
    from pieces.bishop import Bishop
    from pieces.rook import Rook
    from pieces.king import King
    from pieces.pawn import Pawn
    from pieces.knight import Knight
    from pieces.queen import Queen
    
    straight_directions = [(1, 0), (-1, 0), (0, 1), (0, -1)]
    diagonal_directions = [(1, 1), (-1, 1), (1, -1), (-1, -1)]
    knight_positions = [(2, 1), (1, 2), (-1, 2), (-2, 1), (-2, -1), (-1, -2), (1, -2), (2, -1)]
    king_positions = [(1, 0), (-1, 0), (0, 1), (0, -1), (1, 1), (-1, 1), (1, -1), (-1, -1)]
    white_pawn_positions = [(1, 1), (-1, 1)]
    black_pawn_positions = [(1, -1), (-1, -1)]
    
    straight_threatining_pieces = [Rook, Queen]
    diagonal_threatining_pieces = [Bishop, Queen]
    
    for direction in straight_directions:
        if check_threat(direction, opponent_pieces, straight_threatining_pieces, my_king_position, own_pieces_positions):
            return True
    
    for direction in diagonal_directions:
        if check_threat(direction, opponent_pieces, diagonal_threatining_pieces, my_king_position, own_pieces_positions):
            return True
    
    for position in knight_positions:
        knight_new_position = my_king_position[0] + position[0], my_king_position[1] + position[1]
        if knight_new_position in own_pieces_positions:
            continue
        for piece in opponent_pieces:
            if piece.position == knight_new_position and type(piece) == Knight:
                return True
    
    for position in king_positions:
        king_new_position = my_king_position[0] + position[0], my_king_position[1] + position[1]
        for piece in opponent_pieces:
            if piece.position == king_new_position and type(piece) == King:
                return True
            
    if whose_turn == "white":
        for position in black_pawn_positions:
            king_new_position = my_king_position[0] + position[0], my_king_position[1] + position[1]
            for piece in opponent_pieces:
                if piece.position == king_new_position and type(piece) == Pawn:
                    return True        
    else:
        for position in white_pawn_positions:
            king_new_position = my_king_position[0] + position[0], my_king_position[1] + position[1]
            for piece in opponent_pieces:
                if piece.position == king_new_position and type(piece) == Pawn:
                    return True
                
    return False