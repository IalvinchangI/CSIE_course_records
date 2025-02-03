import scrapy
from bs4 import BeautifulSoup

from MLB_Data_Crawling_and_Analysis.items import MlbDataCrawlingAndAnalysisItem


class MlbspiderSpider(scrapy.Spider):
    name = "MLBspider"
    allowed_domains = ["www.mlb.com"]
    start_urls = ["https://www.mlb.com/stats/"]

    def __init__(self):
        self.page_number = 1

    def parse(self, response):
        soup = BeautifulSoup(response.text, "html.parser")

        element = soup.find("div", class_="no-results-message-_31uOqXr")  # find the no result message
        if element != None:
            return

        player_hitting_table = soup.find("tbody", class_="notranslate")  # find the tbody whose class is "notranslate"

        for tr in player_hitting_table.find_all("tr"):
            item = MlbDataCrawlingAndAnalysisItem()

            item["PLAYER"]  = tr.find("th").find("div", class_="top-wrapper-TqtRaIeD").find("div")  # process later in pipelines
            item["R"]       = tr.find("td", attrs={"data-col": "4"}).text 
            item["RBI"]     = tr.find("td", attrs={"data-col": "9"}).text 
            item["H"]       = tr.find("td", attrs={"data-col": "5"}).text 
            item["HR"]      = tr.find("td", attrs={"data-col": "8"}).text 
            item["AVG"]     = tr.find("td", attrs={"data-col": "14"}).text 
            item["OBP"]     = tr.find("td", attrs={"data-col": "15"}).text 
            item["SLG"]     = tr.find("td", attrs={"data-col": "16"}).text 
            item["SB"]      = tr.find("td", attrs={"data-col": "12"}).text 
            item["AB"]      = tr.find("td", attrs={"data-col": "3"}).text 
            item["BB"]      = tr.find("td", attrs={"data-col": "10"}).text 
            item["SO"]      = tr.find("td", attrs={"data-col": "11"}).text 
            item["G"]       = tr.find("td", attrs={"data-col": "2"}).text 
            item["TEAM"]    = tr.find("td", attrs={"data-col": "1"}).text 

            yield item
        
        self.page_number += 1
        yield response.follow(self.start_urls[0] + f"?page={self.page_number}", callback=self.parse)
