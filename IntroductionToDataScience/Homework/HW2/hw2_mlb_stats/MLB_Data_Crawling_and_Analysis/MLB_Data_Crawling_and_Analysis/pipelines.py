# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: https://docs.scrapy.org/en/latest/topics/item-pipeline.html


# useful for handling different item types with a single interface
from itemadapter import ItemAdapter


class MlbDataCrawlingAndAnalysisPipeline:
    def process_item(self, item, spider):
        # player name
        name = item["PLAYER"].find_all("span")
        item["PLAYER"] = name[0].text + " " + name[2].text

        return item
