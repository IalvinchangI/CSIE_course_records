# Define here the models for your scraped items
#
# See documentation in:
# https://docs.scrapy.org/en/latest/topics/items.html

import scrapy


class MlbDataCrawlingAndAnalysisItem(scrapy.Item):
    # define the fields for your item here like:
    PLAYER   = scrapy.Field()  # player                 # th
    R        = scrapy.Field()  # runs_scored (runs)     # 4
    RBI      = scrapy.Field()  # runs_batted_in         # 9
    H        = scrapy.Field()  # hits                   # 5
    HR       = scrapy.Field()  # home_runs              # 8
    AVG      = scrapy.Field()  # batting_average        # 14
    OBP      = scrapy.Field()  # on_base_percentage     # 15
    SLG      = scrapy.Field()  # slugging_percentage    # 16
    SB       = scrapy.Field()  # stolen_bases           # 12
    AB       = scrapy.Field()  # at_bats                # 3
    BB       = scrapy.Field()  # walks                  # 10
    SO       = scrapy.Field()  # strikeouts             # 11
    G        = scrapy.Field()  # games_played           # 2
    TEAM     = scrapy.Field()  # team                   # 1
