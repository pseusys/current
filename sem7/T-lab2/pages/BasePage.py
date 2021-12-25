from typing import Optional

from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.by import By
from selenium.webdriver.remote.webelement import WebElement
from selenium.webdriver.support.expected_conditions import element_to_be_clickable, visibility_of_element_located, any_of
from selenium.webdriver.support.wait import WebDriverWait
from webdriver_manager.chrome import ChromeDriverManager
from selenium.webdriver import Chrome, ChromeOptions

from pages._xpath import create, Tag, Param, Condition


class BasePage:
    def __init__(self, driver=None, username=None, url="", service=Service(ChromeDriverManager().install())):
        self._username = username
        if not driver:
            self._driver = Chrome(service=service)
            self._driver.maximize_window()
            self._driver.get(f"https://github.com/{url}")
        else:
            self._driver = driver

    def _scroll_to_bottom(self):
        self._driver.execute_script("window.scrollTo(0, document.body.scrollHeight);")

    def _navigate(self, link_text: str):
        self._find_visible_enabled(Tag.Link, (Param.Link, Condition.Contains, link_text)).click()

    def _fill_form(self, *fields_values: tuple[str, str]):
        for field, value in fields_values:
            self._find_visible_enabled(Tag.Input, (Param.Name, Condition.Equals, field)).send_keys(value)

    def _await_element_to_appear(self, tag: Tag, *pairs: tuple[Param, Condition, str]) -> WebElement:
        WebDriverWait(self._driver, 5).until(visibility_of_element_located((By.XPATH, create(tag, *pairs))))
        return self._find_visible_enabled(tag, *pairs)

    def _await_element_to_enable(self, tag: Tag, *pairs: tuple[Param, Condition, str]) -> WebElement:
        WebDriverWait(self._driver, 5).until(any_of(*[element_to_be_clickable(elem) for elem in self._driver.find_elements(By.XPATH, create(tag, *pairs))]))
        return self._find_visible_enabled(tag, *pairs)

    def _find_visible_enabled(self, tag: Tag, *pairs: tuple[Param, Condition, str]) -> Optional[WebElement]:
        for elem in self._driver.find_elements(By.XPATH, create(tag, *pairs)):
            if elem.is_enabled() and elem.is_displayed():
                return elem

    @staticmethod
    def _find_children_visible_enabled(elem: WebElement, tag: Tag) -> list[Optional[WebElement]]:
        return [e for e in elem.find_elements(By.XPATH, f".//{tag.value}") if e.is_enabled() and e.is_displayed()]
