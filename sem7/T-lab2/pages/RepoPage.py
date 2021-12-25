from selenium.webdriver import Chrome

import pages
from pages._xpath import Tag, Param, Condition


class RepoPage(pages.BasePage):
    def __init__(self, driver: Chrome, username: str, repo_name: str, *args, **kwargs):
        super(RepoPage, self).__init__(driver, username, f"{username}/{repo_name}", *args, **kwargs)
        self._name = repo_name

    def return_to_main(self) -> pages.MainPage:
        self._navigate("https://github.com/")
        return pages.MainPage(self._driver, self._username)

    def open_settings(self) -> pages.SettingsPage:
        self._navigate(f"/{self._username}/{self._name}/settings")
        return pages.SettingsPage(self._driver, self._username, self._name)

    def has_readme(self) -> bool:
        return self._find_visible_enabled(Tag.Div, (Param.Id, Condition.Equals, "readme")) is not None

    def is_private(self) -> bool:
        return self._find_visible_enabled(Tag.Span, (Param.Class, Condition.Contains, "Label"), (Param.Class, Condition.Contains, "Label--secondary")).text == "Private"

    def get_description(self) -> str:
        return self._find_visible_enabled(Tag.Par, (Param.Class, Condition.Contains, "f4"), (Param.Class, Condition.Contains, "my-3")).text
