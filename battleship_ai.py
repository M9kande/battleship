def choose_move(board, remaining_ships):
    from collections import deque

    def in_bounds(r, c):
        return 0 <= r < 8 and 0 <= c < 8

    def find_hit_clusters():
        visited = [[False]*8 for _ in range(8)]
        clusters = []

        for r in range(8):
            for c in range(8):
                if board[r][c] == 2 and not visited[r][c]:
                    cluster = []
                    queue = deque([(r, c)])
                    visited[r][c] = True

                    while queue:
                        cr, cc = queue.popleft()
                        cluster.append((cr, cc))
                        for dr, dc in [(-1,0), (1,0), (0,-1), (0,1)]:
                            nr, nc = cr + dr, cc + dc
                            if in_bounds(nr, nc) and board[nr][nc] == 2 and not visited[nr][nc]:
                                visited[nr][nc] = True
                                queue.append((nr, nc))

                    clusters.append(cluster)
        return clusters

    def get_target_cells(cluster):
        cluster.sort()
        targets = set()

        if len(cluster) == 1:
            r, c = cluster[0]
            for dr, dc in [(-1,0), (1,0), (0,-1), (0,1)]:
                nr, nc = r + dr, c + dc
                if in_bounds(nr, nc) and board[nr][nc] == 0:
                    targets.add((nr, nc))
        else:
            rows = [r for r, _ in cluster]
            cols = [c for _, c in cluster]
            if all(r == rows[0] for r in rows):
                # horizontal
                r = rows[0]
                min_c, max_c = min(cols), max(cols)
                if in_bounds(r, min_c - 1) and board[r][min_c - 1] == 0:
                    targets.add((r, min_c - 1))
                if in_bounds(r, max_c + 1) and board[r][max_c + 1] == 0:
                    targets.add((r, max_c + 1))
            elif all(c == cols[0] for c in cols):
                # vertical
                c = cols[0]
                min_r, max_r = min(rows), max(rows)
                if in_bounds(min_r - 1, c) and board[min_r - 1][c] == 0:
                    targets.add((min_r - 1, c))
                if in_bounds(max_r + 1, c) and board[max_r + 1][c] == 0:
                    targets.add((max_r + 1, c))
        return targets

    # Create heatmap
    heatmap = [[0 for _ in range(8)] for _ in range(8)]
    for ship_len in remaining_ships:
        for r in range(8):
            for c in range(8):
                if c + ship_len <= 8 and all(board[r][cc] == 0 for cc in range(c, c+ship_len)):
                    for cc in range(c, c+ship_len):
                        heatmap[r][cc] += 1
                if r + ship_len <= 8 and all(board[rr][c] == 0 for rr in range(r, r+ship_len)):
                    for rr in range(r, r+ship_len):
                        heatmap[rr][c] += 1

    # Boost around hits
    clusters = find_hit_clusters()
    for cluster in clusters:
        targets = get_target_cells(cluster)
        for r, c in targets:
            heatmap[r][c] += 100  # big bonus

    # Choose best move
    best = (0, 0)
    max_score = -1
    for r in range(8):
        for c in range(8):
            if board[r][c] == 0 and heatmap[r][c] > max_score:
                best = (r, c)
                max_score = heatmap[r][c]

    return best
