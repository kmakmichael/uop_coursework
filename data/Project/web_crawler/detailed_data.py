import datetime

import requests as rq
from bs4 import BeautifulSoup
import re
import csv

mollusk_url = 'https://mollusk.apis.ign.com/graphql'
ign_url = 'https://www.ign.com'
cookie = 'muxData=mux_viewer_id=4c754e05-07fb-46f0-83af-0671541c90f3&msn=0.19546833795110963&sid=3c3dadb3-ba92-4139-8633-f2ebf256807e&sst=1645218717272&sex=1645220309982; noauth=1645125045177; _dd_s=logs=1&id=b67de6d7-5ec9-4136-8c52-6f90434f2bf0&created=1645424322996&expire=1645425239246; spotim_visitId={%22visitId%22:%2278624ced-0ada-4bde-9a27-0f012a1873d5%22%2C%22creationDate%22:%222022-02-21T06:18:43.051Z%22%2C%22duration%22:0'
sha = 'f6f4ba2432e75e119b1c47300d99cc0aad92443aedf0b3c5ddc493d57f7d8680'

hdrs = {
    "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8",
    "Accept-Encoding": "gzip, deflate, br",
    "Accept-Language": "en-US,en;q=0.5",
    "Connection": "keep-alive",
    "Cookie": f'{cookie}',
    "DNT": "1",
    "Host": "www.ign.com",
    "Referer": "https://www.ign.com/reviews/games",
    "Sec-Fetch-Dest": "document",
    "Sec-Fetch-Mode": "navigate",
    "Sec-Fetch-Site": "same-origin",
    "Sec-Fetch-User": "?1",
    "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:93.0) Gecko/20100101 Firefox/93.0",
}

fieldnames = ["id",
              "review_title", "score", "phrase", "reviewer",
              "reviewed_on", "review_date", "review_id", "review_url",
              "game_title", "release_date", "platform", "genre", "features",
              "franchise", "developer", "publisher", "game_id", "game_url"]


def lencheck(arr):
    """x = len(arr)
    match x:
        case 1: return arr[0]
        case 0: return ""
        case _: return arr"""
    if len(arr) == 1:
        return arr[0]
    elif len(arr) == 0:
        return ""
    else:
        return arr


def responsecheck(g, r):
    icheck = (isinstance(g, dict), isinstance(r, dict))
    if icheck[0] and icheck[1]:
        return True
    if not icheck[0]:
        print(f'\t-->game error: {g}')
    if not icheck[1]:
        print(f'\t-->review error: {r}')
    return False


def game_details(s, subdomain):
    deets = {}
    # get the page
    reply = s.get(f'{ign_url}{subdomain}', headers=hdrs)
    if reply.status_code != 200:
        return reply.status_code
    soup = BeautifulSoup(reply.content, "html.parser")

    # Title
    title = soup.find('h1', class_=re.compile("^display-title"))
    deets["game_title"] = title.string

    deetbox = soup.find(class_=re.compile("object-info$"))  # narrow our search
    # Developers
    devs = deetbox.find(attrs={'data-cy': 'developers-info'})
    dbox = []
    if devs is not None:
        for d in devs.find_all('a'):
            dbox.append(d.string)
    deets["developer"] = lencheck(dbox)
    # Publishers
    publishers = deetbox.find(attrs={'data-cy': 'publishers-info'})
    pbox = []
    if publishers is not None:
        for p in publishers.find_all('a'):
            pbox.append(p.string)
    deets["publisher"] = lencheck(pbox)
    # Franchises
    franchises = deetbox.find(attrs={'data-cy': 'franchises-info'})
    fbox = []
    if franchises is not None:
        for f in franchises.find_all('a'):
            fbox.append(f.string)
    deets["franchise"] = lencheck(fbox)
    # Features
    features = deetbox.find(attrs={'data-cy': 'features-info'})
    tbox = []
    if features is not None:
        for f in features.find_all('a'):
            tbox.append(f.string)
    deets["features"] = lencheck(tbox)
    # Release Date
    date = deetbox.find(attrs={'data-cy': 'release-date-info'})
    if date is not None:
        datestr = date.find_all('div')[1].string
        deets["release_date"] = datetime.datetime.strptime(datestr, '%B %d, %Y').date().strftime("%Y-%m-%d")
    else:
        deets["release_date"] = ""

    # Platforms
    platforms = deetbox.find(attrs={'data-cy': 'platforms-info'})
    cbox = []
    if platforms is not None:
        for p in platforms.find_all('a'):
            cbox.append(p.string)
    deets["platform"] = lencheck(cbox)

    # Genres
    genres = deetbox.find(attrs={'data-cy': 'genres-info'})
    gbox = []
    if genres is not None:
        for g in genres.find_all('a'):
            gbox.append(g.string)
    deets["genre"] = lencheck(gbox)
    return deets

def review_details(s, subdomain):
    deets = {}
    # get the page
    reply = s.get(f'{ign_url}{subdomain}', headers=hdrs)
    if reply.status_code != 200:
        return reply.status_code
    soup = BeautifulSoup(reply.content, "html.parser")

    # Title
    headline = soup.find('div', class_=re.compile("article-headline$"))
    if headline is not None:
        deets["review_title"] = headline.find('h1').string
    else:
        deets["review_title"] = ""
    # Score
    score = soup.find('span', class_=re.compile("hexagon-content$"))
    if score is not None:
        deets["score"] = score.string
    else:
        deets["score"] = ""
    # Score Phrase
    phrase = soup.find('div', class_=re.compile("score-text$"))
    if phrase is not None:
        deets["phrase"] = phrase.string
    else:
        deets["phrase"] = ""
    # Reviewed On
    ron = soup.find('div', class_=re.compile("reviewedOn$")).string
    if ron is not None:
        deets["reviewed_on"] = ron.split(": ")[1]
    else:
        deets["reviewed_on"] = ""
    # Date
    date = soup.find('div', class_=re.compile("article-publish-date$"))
    if date is not None:
        datestr = date.contents[len(date.contents)-1]
        deets["review_date"] = datetime.datetime.strptime(datestr, "%d %b %Y %I:%M %p").strftime("%Y-%m-%d")
    else:
        deets["review_date"] = ""
    # Reviewer(s)
    phrase = soup.find('section', class_=re.compile("author-names$"))
    rbox = []
    if phrase is not None:
        for a in phrase.find_all('a'):
            rbox.append(a.string)
    deets["reviewer"] = lencheck(rbox)
    return deets

if __name__ == '__main__':
    s = rq.Session()
    idx = 0
    with open('resources.csv', newline='', encoding='utf-8') as resources:
        with open('full_data.csv', 'w', newline='', encoding='utf-8') as outfile:
            reader = csv.DictReader(resources)
            writer = csv.DictWriter(outfile, fieldnames=fieldnames)
            writer.writeheader()
            for row in reader:
                print(f'{idx}: ({row["review_url"]}, {row["game_url"]})')
                g = game_details(s, row["game_url"])
                r = review_details(s, row["review_url"])
                if not responsecheck(g, r):
                    continue
                entry = row | g | r
                entry["id"] = idx
                writer.writerow(entry)
                idx += 1
