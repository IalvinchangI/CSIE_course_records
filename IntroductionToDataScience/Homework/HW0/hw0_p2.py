# hw0 p1 Movie Data Analysis
# author IalvinchangI


MULTI  = ("Genre", "Actors")
NUMBER = ("Rank", "Year", "Runtime (Minutes)", "Rating", "Votes", "Revenue (Millions)", "Metascore")

COLUMN_SEP = ","
INNER_SEP  = "|"

def strip_all(iterable: list[str]) -> tuple:
    return tuple(map(lambda x: x.strip(), iterable))


# read file
with open("IMDB-Movie-Data.csv", mode="r") as data_file:
    tabel_head = strip_all(data_file.readline().split(COLUMN_SEP))  # tabel head
    datas: list[dict] = list()           # depend on movie
    actors_info: dict[dict] = dict()     # depend on actor
    directors_info: dict[set] = dict()   # depend on director
    for movie in data_file:
        movie_data = strip_all(movie.split(COLUMN_SEP))

        if len(movie_data) != len(tabel_head):
            continue

        # organize
        data = dict(zip(tabel_head, movie_data))
        for i in range(len(MULTI)):
            data[MULTI[i]] = strip_all(data[MULTI[i]].split(INNER_SEP))

        for i in range(len(NUMBER)):
            if data[NUMBER[i]] == "":
                data[NUMBER[i]] = None
            else:
                data[NUMBER[i]] = float(data[NUMBER[i]])

        # store
        datas.append(data)  # depend on movie

        # depend on actor
        for actor in data["Actors"]:
            if actor not in actors_info:
                actors_info[actor] = {"collaborate_actors": set(), "genres": set(), "revenue": set(), "year": set()}
            actors_info[actor]["collaborate_actors"].update(data["Actors"])
            actors_info[actor]["collaborate_actors"].remove(actor)
            actors_info[actor]["genres"].update(data["Genre"])
            if data["Revenue (Millions)"] != None:
                actors_info[actor]["revenue"].add(data["Revenue (Millions)"])
            if data["Year"] != None:
                actors_info[actor]["year"].add(data["Year"])

        # depend on director
        if data["Director"] not in directors_info:
            directors_info[data["Director"]] = set()
        directors_info[data["Director"]].update(data["Actors"])


def getTop(list_like, key=lambda x: x, top=3) -> list:
    """ list_like: [great -> small] """
    output = list(list_like[:top])

    top_element_key = key(list_like[top - 1])
    for element in list_like[top:]:
        if top_element_key > key(element):
            break
        output.append(element)  # top_element_key <= key(element)
    
    return output


# question
def getTop3Movies2016(datas):  # 1
    print(
        "Top-3 movies with the highest ratings in 2016:", 
        ", ".join(
            data["Title"] for data in getTop(sorted(
                (data for data in datas if data["Year"] == 2016), reverse=True, key=lambda data: data["Rating"]
            ), key=lambda data: data["Rating"])  # get top 3 by sorted all the datas
        )
    )

def getTopRevenueActor(actors_info):  # 2
    # average revenue = sum(revenue) / (the number of movies this actor has appeared in)
    print(
        "The actor generating the highest average revenue:", 
        ", ".join(
            actor_info[0] for actor_info in getTop(
                sorted((
                    (
                        actor_info[0], 
                        sum(actor_info[1]["revenue"]) / len(actor_info[1]["revenue"]) if (len(actor_info[1]["revenue"]) != 0) else -1
                    )
                    for actor_info in actors_info.items()
                ), reverse=True, key=lambda actor_info: actor_info[1]), 
                key=lambda actor_info: actor_info[1], top=1
            )
        )
    )

def getAverageRatingEmmaWatsonMovies(datas):  # 3
    Emma_Watson_movie = tuple(data["Rating"] for data in datas if "Emma Watson" in data["Actors"])
    print(
        "The average rating of Emma Watson’s movies:", 
        sum(Emma_Watson_movie) / len(Emma_Watson_movie)
    )

def getTopDirectorsByActorCollaboration(directors_info):  # 4
    print(
        "Top-3 directors who collaborate with the most actors:", 
        ", ".join(
            director[0] for director in getTop(sorted(
                directors_info.items(), reverse=True, key=lambda director: len(director[1])
            ), key=lambda director: len(director[1]))  # get top 3 by sorted all the datas
        )
    )

def getTopActorsByMovieGenres(actors_info):  # 5
    print(
        "Top-2 actors playing in the most genres of movies:", 
        ", ".join(
            actor[0] for actor in getTop(sorted(
                actors_info.items(), reverse=True, key=lambda actor: len(actor[1]["genres"])
            ), key=lambda actor: len(actor[1]["genres"]))  # get top 2 by sorted all the datas
        )
    )

def getTopActorsByMaxYearGap(actors_info):  # 6
    print(
        "Top-1 actors whose movies lead to the largest maximum gap of years:", 
        ", ".join(
            actor_info[0] for actor_info in getTop(
                sorted((
                    (actor_info[0], max(actor_info[1]["year"]) - min(actor_info[1]["year"])) for actor_info in actors_info.items()
                ), reverse=True, key=lambda actor_info: actor_info[1]), 
                key=lambda actor_info: actor_info[1], top=1
            )  # get top 1 by sorted all the datas
        )
    )

def findActorsCollaboratingWithJohnnyDepp(actors_info):  # 7
    final_collaborate_actors_set = actors_info["Johnny Depp"]["collaborate_actors"]
    final_collaborate_actors_list = list(final_collaborate_actors_set)

    for actor in final_collaborate_actors_list:
        difference = actors_info[actor]["collaborate_actors"] - final_collaborate_actors_set  # get difference between 2 sets
        if difference:  # difference != {}
            final_collaborate_actors_list += difference
            final_collaborate_actors_set |= difference
    
    final_collaborate_actors_list.remove("Johnny Depp")
    
    print(
        "All actors who collaborate with Johnny Depp in direct and indirect ways:", 
        ", ".join(final_collaborate_actors_list), 
    )


getTop3Movies2016(datas)
getTopRevenueActor(actors_info)
getAverageRatingEmmaWatsonMovies(datas)
getTopDirectorsByActorCollaboration(directors_info)
getTopActorsByMovieGenres(actors_info)
getTopActorsByMaxYearGap(actors_info)
findActorsCollaboratingWithJohnnyDepp(actors_info)
