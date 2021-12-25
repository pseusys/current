from selenium.webdriver import Chrome

from pages._xpath import Tag, Param, Condition
import pages


class SettingsPage(pages.BasePage):
    def __init__(self, driver: Chrome, username: str, repo_name: str, *args, **kwargs):
        super(SettingsPage, self).__init__(driver, username, f"{username}/{repo_name}/settings", *args, **kwargs)
        self._name = repo_name

    def delete_repo(self) -> pages.MainPage:
        self._scroll_to_bottom()
        self._await_element_to_appear(Tag.Summary, (Param.Text, Condition.Contains, "Delete this repository")).click()

        self._fill_form(("verify", f"{self._username}/{self._name}"))
        self._await_element_to_enable(Tag.Button, (Param.Type, Condition.Equals, "submit"), (Param.Class, Condition.Contains, "btn-danger")).click()
        return pages.MainPage(self._driver, self._username)
