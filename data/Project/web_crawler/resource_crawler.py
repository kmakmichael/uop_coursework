import requests as rq
import csv

url = 'https://mollusk.apis.ign.com/graphql'
cookie = "muxData=mux_viewer_id=4c754e05-07fb-46f0-83af-0671541c90f3&msn=0.19546833795110963&sid=3c3dadb3-ba92-4139-8633-f2ebf256807e&sst=1645218717272&sex=1645220309982; noauth=1645125045177; _dd_s=logs=1&id=b67de6d7-5ec9-4136-8c52-6f90434f2bf0&created=1645424322996&expire=1645425239246; spotim_visitId={%22visitId%22:%2278624ced-0ada-4bde-9a27-0f012a1873d5%22%2C%22creationDate%22:%222022-02-21T06:18:43.051Z%22%2C%22duration%22:0"
sha = "f6f4ba2432e75e119b1c47300d99cc0aad92443aedf0b3c5ddc493d57f7d8680"
s = rq.Session()
hd = {
    "Accept": "*/*",
    "Accept-Encoding": "gzip, deflate, br",
    "Accept-Language": "en-US,en;q=0.5",
    "apollographql-client-name": "kraken",
    "apollographql-client-version": "v0.11.11",
    "Connection": "keep-alive",
    "content-type": "application/json",
    "Cookie": f'{cookie}',
    "DNT": "1",
    "Host": "mollusk.apis.ign.com",
    "If-None-Match": "2a0f-yvx2Ad7x3viOeXo7GnD3s57Eskc",
    "Origin": "https://www.ign.com",
    "Referer": "https://www.ign.com/reviews/games",
    "Sec-Fetch-Dest": "empty",
    "Sec-Fetch-Mode": "cors",
    "Sec-Fetch-Site": "same-site",
    "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:93.0) Gecko/20100101 Firefox/93.0",
    "x-postgres-articles": "true"
}
variables = {
    "filter": "Games",
    "region": "us",
    "startIndex": 0,
    "count": 200,
    "editorsChoice": False,
    "sortOption": "Title",
    "gamePlatformSlugs": [],
    "genreSlugs": []
}
extensions = {
    "persistedQuery": {
        "version": 1,
        "sha256Hash": sha
    }
}
extstr = str(extensions).replace(" ", "").replace('\'', '\"')

idx = 0
with open('resources.csv', 'w', newline='', encoding='utf-8') as csvfile:
    fieldnames = ["review_id", "review_url", "game_id", "game_url"]
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
    writer.writeheader()

    for i in range(0, 2000, 200):
        print(f"Processing indices {i}-{i + 200} of est. 18971...")
        variables["startIndex"] = i
        varstr = str(variables).replace(" ", "").replace('\'', '\"').replace("False", 'false')
        addons = f'?operationName=ReviewsContentFeed&variables={varstr}&extensions={extstr}'

        # send an OPTIONS and a GET
        reply = s.options(f'{url}{addons}', headers=hd)
        reply = s.get(f'{url}{addons}', headers=hd)

        # process the json
        feed = reply.json()["data"]["reviewContentFeed"]  # shortcut
        for entry in feed["contentItems"]:
            ec = entry["content"]  # small shortcuts
            writer.writerow({
                'review_id': ec["id"],
                'review_url': ec["url"],
                'game_id': ec["primaryObject"]["id"],
                'game_url': ec["primaryObject"]["url"]
            })
            """# process the reviewers
            revs = []
            for r in ec["contributors"]:
                revs.append(r["name"])
            if len(r) == 0:
                r.append("IGN Staff")

            # write to csv
            writer.writerow({
                'id': idx,
                'url': ec["url"],
                'reviewers': revs,
                'date': ec["publishDate"],
                'game': ec["title"],
                'score': entry["review"]["score"]
            })"""
            idx += 1
        if not feed["pagination"]["isMore"]:
            print("Reached end of dataset!")
            break